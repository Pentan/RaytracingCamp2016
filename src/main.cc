#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <cstdio>
#include <string>
#include <thread>
#include <chrono>

#include "r1htypes.h"
#include "scene.h"
#include "renderer.h"
#include "framebuffer.h"
#include "tonemapper.h"
#include "xmlscene.h"

//
#include "eduptscene.h"
//

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
static double gettimeofday_sec() {
	return timeGetTime() / 1000.0;
}
#else
#include <sys/time.h>
static double gettimeofday_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec * 1e-6;
}
#endif

///
//#define kProgressOutIntervalSec	30.0
static const char kProgressChars[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ(){}[]<>!#$%&=^~@+*?/_";

///
int main(int argc, char *argv[]) {

	std::cout << "RrT" << std::endl;

    using namespace r1h;

    double startTime = gettimeofday_sec();

    // renderer setup
    Renderer *render = new Renderer();

    // scene setup
    Scene *scene = new Scene();
	bool loaded = false;
	
	if(argc > 1){
		XMLSceneLoader loader;
        std::string xmlfile(argv[1]);
		loaded = loader.load(xmlfile, scene, render);
		//loaded = true;
		//return 0;
    } else {
#if 1
        //std::string xmlfile("scenes/edupt_cornelbox.xml");
        //std::string xmlfile("scenes/edupt_cornelbox2.xml");
        //std::string xmlfile("scenes/bsdftest.xml");
        //std::string xmlfile("scenes/materialtest.xml");
        //std::string xmlfile("scenes/materialtest2.xml");
        //std::string xmlfile("scenes/timetest.xml");
        std::string xmlfile("scenes/scene2016.xml");
        
        XMLSceneLoader loader;
        loaded = loader.load(xmlfile, scene, render);
#else
        Renderer::Config conf = render->getConfig();
        conf.width = 256;
        conf.height = 256;
        conf.samples = 32;
        render->setConfig(conf);
        loaded = EduptScene::load(scene, double(conf.width) / conf.height);
        //loaded = true;
#endif
    }

    printf("scene loaded [%.4f sec]\n", gettimeofday_sec() - startTime);

	if(loaded) {
        const Renderer::Config &conf = render->getConfig();

		// set tone mapper
		ToneMapper *mapper = new ToneMapper();
        //mapper->setGamma(1.0); //+++++
		
		// render
		double renderStart = gettimeofday_sec();
		render->render(scene, true); // detach
		
		// wait to finish
		int outcount = 0;
		double prevouttime = gettimeofday_sec();
        size_t progcharlen = strlen(kProgressChars);
        
        int numcntx = (int)render->getRecderContextCount();
		do {
			double progress = render->getRenderProgress();
			
			std::this_thread::sleep_for(std::chrono::seconds(1));
			
			double curtime = gettimeofday_sec();
			if(conf.progressOutInterval > 0.0 && curtime - prevouttime > conf.progressOutInterval) {
				// progress output
				char buf[16];
				sprintf(buf, "%03d.bmp", outcount);

				mapper->exportBMP(render->getFrameBuffer(), buf);
                printf("progress image %s saved\n", buf);
				outcount++;
				prevouttime += conf.progressOutInterval;
			}
			
            // print progress log
			printf("%.2lf%%:", progress);
            for(int i = 0; i < numcntx; i++) {
                const Renderer::Context *cntx = render->getRenderContext(i);
                //printf("[%.1lf]", cntx->tileProgress * 100.0);
                size_t progindex = size_t(cntx->tileProgress * progcharlen);
                if(progindex >= progcharlen) {
                    progindex = progcharlen - 1;
                }
                printf("%c", kProgressChars[progindex]);
            }
            printf("    \r");
            fflush(stdout);
            
            // timelimit mode
            if(conf.renderMode == Renderer::kTimeLimit) {
                double pasttime = curtime - startTime;
                if(pasttime > conf.maxLimitTime) {
                    for(int i = 0; i < numcntx; i++) {
                        Renderer::Context *cntx = render->getRenderContext(i);
                        cntx->killRequest = true;
                    }
                }
            }
            
		} while( !render->isFinished() );

		printf("render finished (%.4f sec) [%.4f sec]\n", gettimeofday_sec() - renderStart, gettimeofday_sec() - startTime);
		
		// final image
		Renderer::Config renderConf = render->getConfig();
		const char *finalname = renderConf.outputFile.c_str();//"final.bmp";
		mapper->exportBMP(render->getFrameBuffer(), finalname);
        printf("%s saved\n", finalname);

		printf("saved [%.4f sec]\n", gettimeofday_sec() - startTime);
		
		delete mapper;
	}

    // cleaning
    delete render;
    delete scene;

    printf("done [%.4f sec]\n", gettimeofday_sec() - startTime);

    return 0;
}
