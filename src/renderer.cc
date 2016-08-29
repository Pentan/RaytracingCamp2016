#include <iostream>
#include <chrono>
#include <random>

#include "renderer.h"
#include "scene.h"
#include "framebuffer.h"
#include "commandqueue.h"
#include "ray.h"
#include "camera.h"

using namespace r1h;

/////

Renderer::Context::Context():
random(0),
state(kAwake),
killRequest(false)
{}

Renderer::Context::~Context() {}

void Renderer::Context::init(const unsigned int seed, const Config *conf) {
    random.setSeed(seed);
    rayVector1.reserve(128);
    rayVector2.reserve(128);
    workVector.reserve(32);
    config = conf;
    
    curInsidentRay = nullptr;
    
    insidentRays = &rayVector1;
    emittedRays = &rayVector2;
}

void Renderer::Context::startWithRay(const Ray& ray) {
    insidentRays->push_back(ray);
    traceDepth = 0;
}

size_t Renderer::Context::numInsidentRays() {
    return insidentRays->size();
}
void Renderer::Context::startRayIteration() {
    iterationCount = 0;
}
bool Renderer::Context::isEndRayIteration() {
    return iterationCount >= insidentRays->size();
}
const Ray& Renderer::Context::nextInsidentRay() {
    Ray& ray = insidentRays->at(iterationCount);
    iterationCount += 1;
    curInsidentRay = &ray;
    return ray;
}

const Ray& Renderer::Context::getCurrentInsidentRay() const {
    return *curInsidentRay;
}

int Renderer::Context::currentTraceDepth() const {
    return traceDepth;
}

void Renderer::Context::setRussianRouletteProbability(R1hFPType p) {
    russianRoulette = p;
}
/*
void Renderer::Context::calcIncidentWeight(const FinalIntersection &isect) {
    hitNormal = isect.shadingNormal;
}
*/
Ray& Renderer::Context::emitRay(const Vector3 &orig, const Vector3 &dir, const Vector3 &weight, const int bsdftype) {
    Ray ray(orig, dir, weight);
    return emitRay(ray, Color(1.0), bsdftype);
}
Ray& Renderer::Context::emitRay(const Ray &newray, const Color &reflectance, const int bsdftype) {
    emittedRays->push_back(newray);
    Ray& retray = emittedRays->back();
    retray.weight = Vector3::mul(retray.weight, reflectance);
    retray.weight = Vector3::mul(retray.weight, curInsidentRay->weight);
    retray.weight = retray.weight / russianRoulette;
    retray.fromBSDF = bsdftype;
    
    return retray;
}

void Renderer::Context::swapRayBuffers() {
    std::swap(insidentRays, emittedRays);
    emittedRays->clear();
    traceDepth += 1;
}

/////

Renderer::Renderer():
    frameBuffer(0),
    config(),
    renderQueue(0),
    renderContexts(0)
{
    // alloc workers
}

Renderer::~Renderer() {
    if( frameBuffer ) {
        delete frameBuffer;
        frameBuffer = nullptr;
    }
    if( renderQueue ) {
        delete renderQueue;
        renderQueue = nullptr;
    }
    if(renderContexts) {
        delete renderContexts;
        renderContexts = nullptr;
    }
}
    
void Renderer::render(Scene *scene, bool isdetach) {
	// framebuffer
	frameBuffer = new FrameBuffer(config.width, config.height);
	
	// scene
	scene->prepareRendering();

	// queue
	renderQueue = new RenderCommandQueue();
    
    // timelimit mode setting
    if(config.renderMode == kTimeLimit) {
        RenderCommandQueue::Command cmd;
        cmd.type = RenderCommandQueue::kSleep;
        cmd.usec = 1;
        renderQueue->defineTernimeter(cmd);
    }
    
	// push tile command
    setRenderTileCommands();
	
    // detect workers
    int numthreads = std::thread::hardware_concurrency();
    if(numthreads <= 0) {
        numthreads = config.defaultThreads;
    }
    if(config.maxThreads > 0 && numthreads > config.maxThreads) {
        numthreads = config.maxThreads;
    }
    
    workers.resize(numthreads);
	//printf("%d threads\n", numthreads);
    
	// init contexts and workers
	renderContexts = new std::vector<Context>(numthreads);
	std::random_device rnddvice;
	for(int i = 0; i < numthreads; ++i) {
		// context
		Context *cntx = &renderContexts->at(i);
		cntx->init(rnddvice(), &config);
		// worker
		workers[i] = std::thread(startWorker, this, i, scene);
	}
	
	if(isdetach) {
		for(int i = 0; i < numthreads; ++i) {
			workers[i].detach();
		}
	}
	else {
		for(int i = 0; i < numthreads; ++i) {
			workers[i].join();
		}
	}
	/*
    //+++++
	{
		//workerJob(0, scene);
		Renderer::startWorker(this, 0, scene);
	}
    //+++++
    */
	
    // finish render
    //printf("render!\n");
};

double Renderer::getRenderProgress() const {
	return ((pushedCommandCount - (int)renderQueue->getRemainCommandCount()) * 100.0) / pushedCommandCount;
}

size_t Renderer::getRecderContextCount() const {
    return renderContexts->size();
}

Renderer::Context* Renderer::getRenderContext(int cntxid) {
    return &renderContexts->at(cntxid);
}

bool Renderer::isFinished() const {
    bool isdone = true;
    for(size_t i = 0; i < renderContexts->size(); i++) {
        isdone &= renderContexts->at(i).state == kDone;
    }
    return isdone;
}

void Renderer::workerJob(int workerId, Scene *scene) {
    // get tile and render!
    
	Context *cntx = &renderContexts->at(workerId);
	
    cntx->state = kWorking;
    while(cntx->state == kWorking) {
        RenderCommandQueue::Command cmd = popRenderCommand();
        
        switch(cmd.type) {   
            case RenderCommandQueue::CommandType::kTile:
                // render tile
                renderTile(cntx, scene, cmd.tile);
                break;
            case RenderCommandQueue::kSleep:
                // sleep
                std::this_thread::sleep_for(std::chrono::microseconds(cmd.usec));
                break;
            case RenderCommandQueue::kFinish:
                cntx->state = kDone;
                break;
            default:
                printf("not handled render command 0x%x\n", cmd.type);
                cntx->state = kDone;
        }
        
        if(cntx->killRequest) {
            cntx->state = kDone;
        }
    }
}

void Renderer::setRenderTileCommands() {
    int w = frameBuffer->getWidth();
    int h = frameBuffer->getHeight();
    
    std::lock_guard<std::mutex> lock(*renderQueue->getMutex());
    
    pushedCommandCount = 0;
    for (int ismpl = 0; ismpl < config.samples; ismpl++) {
        for (int iy = 0; iy < h; iy += config.tileSize) {
            for (int ix = 0; ix < w; ix += config.tileSize) {
                FrameBuffer::Tile tile = frameBuffer->makeTile(ix, iy, config.tileSize, config.tileSize);
                renderQueue->pushTileCommand(tile);
                ++pushedCommandCount;
            }
        }
    }
    
    //std::cout << pushedCommandCount << " tile commands pushed" << std::endl;
}

RenderCommandQueue::Command Renderer::popRenderCommand() {
    RenderCommandQueue::Command cmd = renderQueue->popCommand();
    
    if(renderQueue->isQueueEmpty() && config.renderMode == kTimeLimit) {
        setRenderTileCommands();
    }
    
    return cmd;
}

void Renderer::renderTile(Context *cntx, Scene *scene, FrameBuffer::Tile tile) {
	//std::cout << "[" << std::this_thread::get_id() << "]";
	//printf("render tile (x(%d, %d),y(%d, %d))(size(%d, %d))\n", tile.startx, tile.endx, tile.starty, tile.endy, tile.endx - tile.startx, tile.endy - tile.starty);
    
    int ss = config.subSamples;
    R1hFPType ssrate = 1.0 / ss;
    R1hFPType divw = 1.0 / config.width;
    R1hFPType divh = 1.0 / config.height;
    
    Camera *camera = scene->getCamera();
    
    int numpixels = (tile.endx - tile.startx) * (tile.endy - tile.starty);
    int donepixel = 0;
    cntx->tileProgress = 0.0;
	
    for(int iy = tile.starty; iy < tile.endy; iy++) {
        for(int ix = tile.startx; ix < tile.endx; ix++) {
            for(int ssy = 0; ssy < ss; ssy++) {
                for(int ssx = 0; ssx < ss; ssx++) {
                    R1hFPType px = ix + (ssx + cntx->random.nextf()) * ssrate;
                    R1hFPType py = iy + (ssy + cntx->random.nextf()) * ssrate;
                    R1hFPType cx = px * divw * 2.0 - 1.0;
                    R1hFPType cy = py * divh * 2.0 - 1.0;
					
					Ray ray = camera->getRay(cx, cy, &cntx->random);
#if 1
                    Color c = computeRadiancePT(cntx, scene, ray);
#else
                    Color c = computeRadianceNEE(cntx, scene, ray);
#endif
					frameBuffer->accumulate(ix, iy, c);
                }
            }
            ++donepixel;
            cntx->tileProgress = double(donepixel) / numpixels;
        }
    }
}

Color Renderer::computeRadiancePT(Context *cntx, Scene *scene, const Ray &ray) {
    //Random *crnd = &cntx->random;
    
    //std::vector<Ray> *currays = &cntx->rayVector1;
    //std::vector<Ray> *nextrays = &cntx->rayVector2;
    //std::vector<Ray> *tmprays = &cntx->workVector;
    //tmprays->clear();
    
    // init radiance
    Color radiance(0.0, 0.0, 0.0);
    
    // sky
    SkyMaterial *skymat = scene->getSkyMaterial();
    
    // trace start
    cntx->startWithRay(ray);
    int depth = 0;
    while(cntx->numInsidentRays() > 0) {
        
        // trace
        int minDepth = cntx->config->minDepth;
        int depthLimit = cntx->config->maxDepth;
        
        int raynum = (int)cntx->numInsidentRays();
        cntx->startRayIteration();
        for(int i = 0; i < raynum; i++) {
            const Ray &inray = cntx->nextInsidentRay();
            
            Intersection intersect;
            
            // not intersected. pick background
            if(!scene->isIntersect(inray, &intersect)) {
                Color bgcol(0.0);
                if(skymat != nullptr) {
                    bgcol = skymat->skyColor(inray);
                };
                radiance += Color::mul(bgcol, inray.weight);
                continue;
            }
            
            // hit!
            FinalIntersection fisect(intersect);
            const SceneObject *hitobject = scene->getObject(intersect.objectId);
            fisect.objectRef = hitobject;
            
            // from hit face info
            const Material *hitmat = hitobject->getMaterialById(intersect.materialId);
            fisect.material = hitmat;
            fisect.computeTangentSpaceWithShadingNormal(hitmat->getShadingNormal(fisect));
            
            const Color emittance = hitmat->getEmittance(fisect);
#if 1
            radiance += Color::mul(emittance, inray.weight);
#else
            //+++++
            // for debugging
            // normal
            radiance += fisect.shadingNormal * 0.5 + Vector3(0.5);
            continue;
            // color
            //radiancecol += albedocol;
            //continue;
            //+++++
#endif
            // trace next
            R1hFPType russianprob = hitmat->getTerminationProbability(fisect);
            //std::max(reflectance.r, std::max(reflectance.g, reflectance.b));
            
            if(depth < minDepth) {
                russianprob = 1.0;
            } else {
                russianprob = std::min(1.0, russianprob * inray.weight.getMaxComponent()); // how about this?
                
                if(depth > depthLimit) {
                    russianprob *= pow(0.5, depth - depthLimit);
                }
                if(cntx->random.next01() >= russianprob) {
                    // russian roulette kill
                    continue;
                }
            }
            cntx->setRussianRouletteProbability(russianprob);
            
            hitmat->makeNextSampleRays(cntx, fisect, depth);
        }
        
        cntx->swapRayBuffers();
        ++depth;
    }
    
    return radiance;
}

// Next Event Estimation
Color Renderer::computeRadianceNEE(Context *cntx, Scene *scene, const Ray &ray) {
    // init radiance
    Color radiance(0.0, 0.0, 0.0);
    
    // sky
    SkyMaterial *skymat = scene->getSkyMaterial();
    
    // trace start
    cntx->startWithRay(ray);
    int depth = 0;
    while(cntx->numInsidentRays() > 0) {
        
        // trace
        int minDepth = cntx->config->minDepth;
        int depthLimit = cntx->config->maxDepth;
        
        int raynum = (int)cntx->numInsidentRays();
        cntx->startRayIteration();
        for(int i = 0; i < raynum; i++) {
            const Ray &inray = cntx->nextInsidentRay();
            
            Intersection intersect;
            
            // not intersected. pick background
            if(!scene->isIntersect(inray, &intersect)) {
                Color bgcol(0.0);
                if(skymat != nullptr) {
                    bgcol = skymat->skyColor(inray);
                };
                radiance += Color::mul(bgcol, inray.weight);
                continue;
            }
            
            // hit!
            FinalIntersection fisect(intersect);
            const SceneObject *hitobject = scene->getObject(intersect.objectId);
            fisect.objectRef = hitobject;
            
            // from hit face info
            const Material *hitmat = hitobject->getMaterialById(intersect.materialId);
            fisect.material = hitmat;
            fisect.computeTangentSpaceWithShadingNormal(hitmat->getShadingNormal(intersect));
            
            // hit to light
            if(hitmat->isLight()) {
                // some case, integrate light emittance
                if(inray.fromBSDF == BSDF::kSensor || (inray.fromBSDF & (BSDF::kSpecular | BSDF::kRefraction)) != 0) {
                    const Color emittance = hitmat->getEmittance(fisect);
                    radiance += Color::mul(emittance, inray.weight);
                }
                continue;
            }
            
            // surface emittance
            const Color emittance = hitmat->getEmittance(fisect);
#if 1
            radiance += Color::mul(emittance, inray.weight);
#else
            //+++++
            // for debugging
            // normal
            radiance += fisect.shadingNormal * 0.5 + Vector3(0.5);
            continue;
            // color
            //radiancecol += albedocol;
            //continue;
            //+++++
#endif
            // light sample
            int numLights = scene->getLightObjectsCount();
            Color directlighting(0.0);
            
            /*
            if(depth < 2) {
                // sample all lights
                for(int i = 0; i < numLights; i++) {
                    Color lc = evaluateShadowRay(cntx, scene, i, fisect);
                    directlighting += lc;
                }
                
            } else {
                // choose 1 light
                int i = numLights * cntx->random.nextf();
                R1hFPType lpdf = 1.0 / numLights;
                Color lc = evaluateShadowRay(cntx, scene, i, fisect) / lpdf;
                directlighting += lc;
            }
            */
            // sample all lights
            for(int i = 0; i < numLights; i++) {
                Color lc = evaluateShadowRay(cntx, scene, i, fisect);
                directlighting += lc;
            }
            radiance += Color::mul(directlighting, inray.weight);
            
            // trace next
            R1hFPType russianprob = hitmat->getTerminationProbability(fisect);
            //std::max(reflectance.r, std::max(reflectance.g, reflectance.b));
            
            if(depth < minDepth) {
                russianprob = 1.0;
            } else {
                russianprob = std::min(1.0, russianprob * inray.weight.getMaxComponent()); // how about this?
                
                if(depth > depthLimit) {
                    russianprob *= pow(0.5, depth - depthLimit);
                }
                if(cntx->random.next01() >= russianprob) {
                    // russian roulette kill
                    continue;
                }
            }
            cntx->setRussianRouletteProbability(russianprob);
            
            hitmat->makeNextSampleRays(cntx, fisect, depth);
        }
        
        cntx->swapRayBuffers();
        ++depth;
    }
    
    return radiance;
}

Color Renderer::evaluateShadowRay(Context *cntx, Scene *scene, int lightId, const FinalIntersection &isect) {
    //const Ray &inray = cntx->getCurrentInsidentRay();
    //R1hFPType idotn = Vector3::dot(inray.direction, isect.shadingNormal);
    
    SceneObject *lightobj = scene->getLightObject(lightId);
    Geometry::SamplePoint gsp = lightobj->getSamplePoint(&cntx->random);
    Vector3 dir = gsp.position - isect.position;
    //R1hFPType shadowlen = dir.length();
    dir.normalize();
    
    Ray shadowray(isect.position, dir);
    
    // make offset
    shadowray.origin += dir * kSmallOffset;
    
    Intersection shadowisect;
    if(!scene->isIntersect(shadowray, &shadowisect)) {
        // ?
        std::cerr << "shadow ray hit nothing!" << std::endl;
        return Color(0.0);
    }
    
    // occuluded
    if(shadowisect.objectId != lightobj->objectId) {
        /*
        char origbuf[256];
        char smplbuf[256];
        char hitbuf[256];
        Vector3::sprint(origbuf, shadowray.origin);
        Vector3::sprint(smplbuf, gsp.position);
        Vector3::sprint(hitbuf, shadowisect.position);
        
        printf("%d->%d:(%s)->(%s)=>(%s).%lf\n", isect.objectId, shadowisect.objectId, origbuf, smplbuf, hitbuf, shadowisect.distance);
         */
        return Color(0.0);
    }
    /*
    if(std::abs(shadowisect.distance - shadowlen) > kEPS) {
        // self occulusion
        return Color(0.0);
    }
    */
    // illuminated
    R1hFPType lndot = Vector3::dot(shadowray.direction, shadowisect.hitNormal);
    R1hFPType pndot = Vector3::dot(shadowray.direction, isect.shadingNormal);
    Vector3 vpl = shadowisect.position - isect.position;
    R1hFPType G = std::abs(lndot * pndot) / Vector3::dot(vpl, vpl);
    
    Color ret = isect.material->evalShadowRay(cntx, shadowray, isect) * G / gsp.pdf;
    
    FinalIntersection flisect(shadowisect);
    flisect.material = lightobj->getMaterialById(shadowisect.materialId);
    
    R1hFPType ldotln = std::max(0.0, Vector3::dot(shadowray.direction * -1.0, shadowisect.hitNormal));
    ret = Vector3::mul(ret, flisect.material->getEmittance(flisect) * ldotln);
    
    return ret;
}

void Renderer::startWorker(Renderer *rndr, int workerId, Scene *scene) {
	rndr->workerJob(workerId, scene);
}



