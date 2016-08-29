#ifndef R1H_RENDERER_H
#define R1H_RENDERER_H

#include <thread>
#include <vector>
#include <queue>
#include <string>

#include "r1htypes.h"
#include "random.h"
#include "framebuffer.h"
#include "ray.h"
#include "intersection.h"
#include "commandqueue.h"

namespace r1h {

class Scene;

class Renderer {
public:
    enum RenderMode {
        kStandard,
        kTimeLimit
    };
    
	struct Config {
        int width;
        int height;
        int subSamples;
		int samples;
        int minDepth;
        int maxDepth;
        int defaultThreads;
        int maxThreads;
        int tileSize;
        RenderMode renderMode;
        R1hFPType progressOutInterval;
        R1hFPType maxLimitTime;
		std::string outputFile;
		
        Config(): // make default values
		width(640),
		height(360),
		subSamples(2),
		samples(2),
        minDepth(3),
		maxDepth(32),
		defaultThreads(4),
        maxThreads(0),      // 0 is unlimited
        tileSize(32),
        renderMode(kStandard),
        progressOutInterval(0.0),
        maxLimitTime(0.0),
		outputFile("output.bmp")
        {}
    };
	
    enum RenderState {
        kAwake,
        kWorking,
        kDone
    };
    
	class Context {
    public:
        Random random;
        
		std::vector<Ray> workVector;
		
		const Config *config;
		int state;
        double tileProgress;
        bool killRequest;
        
        Context();
        ~Context();
        void init(const unsigned int seed, const Config *conf);
        
        void startWithRay(const Ray& ray);
        
        size_t numInsidentRays();
        void startRayIteration();
        bool isEndRayIteration();
        const Ray& nextInsidentRay();
        void swapRayBuffers();
        
        const Ray& getCurrentInsidentRay() const;
        int currentTraceDepth() const;
        
        void setRussianRouletteProbability(R1hFPType p);
        //void calcIncidentWeight(const FinalIntersection &isect);
        
        Ray& emitRay(const Vector3 &orig, const Vector3 &dir, const Color &weight, const int bsdftype);
        Ray& emitRay(const Ray &newray, const Color &reflectance, const int bsdftype);
        
    private:
        std::vector<Ray> rayVector1;
        std::vector<Ray> rayVector2;
        std::vector<Ray>* insidentRays;
        std::vector<Ray>* emittedRays;
        size_t iterationCount;
        
        Ray *curInsidentRay;
        int traceDepth;
        R1hFPType russianRoulette;
	};
	
public:
    Renderer();
    ~Renderer();
    
    void render(Scene *scene, bool isdetach=false);
    
    Config getConfig() const { return config; };
    void setConfig(const Config &conf) { config = conf; };
    FrameBuffer* getFrameBuffer() const { return frameBuffer; };
    
	double getRenderProgress() const;
    
    size_t getRecderContextCount() const;
    Context* getRenderContext(int cntxid);
	
    bool isFinished() const;
    
	///
	void workerJob(int workerId, Scene *scene);
private:
    FrameBuffer *frameBuffer;
    Config config;
    
	std::vector<Context> *renderContexts;
    
    // for worker
	int pushedCommandCount;
    RenderCommandQueue *renderQueue;
    std::vector<std::thread> workers;
    
    void setRenderTileCommands();
    
    RenderCommandQueue::Command popRenderCommand();
    void renderTile(Context *cntx, Scene *scene, FrameBuffer::Tile tile);
    Color computeRadiancePT(Context *cntx, Scene *scene, const Ray &ray);
    Color computeRadianceNEE(Context *cntx, Scene *scene, const Ray &ray);
    
    Color evaluateShadowRay(Context *cntx, Scene *scene, int lightId, const FinalIntersection &isect);
    
	static void startWorker(Renderer *rndr, int workerId, Scene *scene);
};

}
#endif
