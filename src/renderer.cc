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
state(kAwake)
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
Ray& Renderer::Context::emitRay(const Vector3 &orig, const Vector3 &dir, const Vector3 &weight) {
    Ray ray(orig, dir, weight);
    return emitRay(ray, Color(1.0));
}
Ray& Renderer::Context::emitRay(const Ray &newray, const Color &reflectance) {
    emittedRays->push_back(newray);
    Ray& retray = emittedRays->back();
    retray.weight = Vector3::mul(retray.weight, reflectance);
    retray.weight = Vector3::mul(retray.weight, curInsidentRay->weight);
    retray.weight = retray.weight / russianRoulette;
    
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
	int w = frameBuffer->getWidth();
	int h = frameBuffer->getHeight();

	// scene
	scene->prepareRendering();

	// queue
	renderQueue = new RenderCommandQueue();

	// push tile command
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
	
    // detect workers
    int numthreads = std::thread::hardware_concurrency();
    if(numthreads <= 0) {
        numthreads = config.defaultThreads;
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

const Renderer::Context* Renderer::getRenderContext(int cntxid) const {
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
        RenderCommandQueue::Command cmd = renderQueue->popCommand();
        
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
    }
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
					Color c = computeRadiance(cntx, scene, ray);
					frameBuffer->accumulate(ix, iy, c);
                }
            }
            ++donepixel;
            cntx->tileProgress = double(donepixel) / numpixels;
        }
    }
}

Color Renderer::computeRadiance(Context *cntx, Scene *scene, const Ray &ray) {
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
            fisect.computeTangentSpaceWithShadingNormal(hitmat->getShadingNormal(intersect));
            
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
            // light sample?
            
            // trace next
            R1hFPType russianprob = hitmat->getTerminationProbability(fisect);
            //std::max(reflectance.r, std::max(reflectance.g, reflectance.b));
            
            if(depth < minDepth) {
                russianprob = 1.0;
            } else {
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

void Renderer::startWorker(Renderer *rndr, int workerId, Scene *scene) {
	rndr->workerJob(workerId, scene);
}



