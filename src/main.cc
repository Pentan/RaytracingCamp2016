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

#ifdef WIN32
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
#define kProgressOutIntervalSec	30.0

///
int main(int argc, char *argv[]) {

	std::cout << "twi" << std::endl;

    using namespace r1h;

    double startTime = gettimeofday_sec();

    // renderer setup
    Renderer *render = new Renderer();

    // scene setup
    Scene *scene = new Scene();
	bool loaded = false;
	
	if(argc > 1){
		XMLSceneLoader loader;
#if 0
		//std::string xmlfile("scenes/edupt_cornelbox.xml");
		//std::string xmlfile("scenes/mesh_cube.xml");
		//std::string xmlfile("scenes/textest.xml");
		//std::string xmlfile("scenes/bsdftest.xml");
		//std::string xmlfile("scenes/mesh_ref.xml");
		//std::string xmlfile("scenes/mesh_cat.xml");
		std::string xmlfile("scenes/scene.xml");
#else
        std::string xmlfile(argv[1]);
#endif
		loaded = loader.load(xmlfile, scene, render);
		//loaded = true;
		//return 0;
    } else {
        Renderer::Config conf = render->getConfig();
        conf.width = 256;
        conf.height = 256;
        conf.samples = 64;
        render->setConfig(conf);
        loaded = EduptScene::load(scene, double(conf.width) / conf.height);
        //loaded = true;
    }

    printf("scene loaded [%.4f sec]\n", gettimeofday_sec() - startTime);

	if(loaded) {

		// set tone mapper
		ToneMapper *mapper = new ToneMapper();
        //mapper->setGamma(1.0); //+++++
		
		// render
		double renderStart = gettimeofday_sec();
		render->render(scene, true); // detach
		
		// wait to finish
		int outcount = 0;
		double prevouttime = gettimeofday_sec();
        
        int numcntx = (int)render->getRecderContextCount();
        
		do {
			double progress = render->getRenderProgress();
			
			std::this_thread::sleep_for(std::chrono::seconds(1));
			
			double curtime = gettimeofday_sec();
			if(curtime - prevouttime > kProgressOutIntervalSec) {
				// progress output
				char buf[16];
				sprintf(buf, "%03d.bmp", outcount);

				mapper->exportBMP(render->getFrameBuffer(), buf);
                printf("progress image %s saved\n", buf);
				outcount++;
				prevouttime += kProgressOutIntervalSec;
			}
			
			printf("%.2lf%%:", progress);
            for(int i = 0; i < numcntx; i++) {
                const Renderer::Context *cntx = render->getRenderContext(i);
                printf("[%d:%.1lf]", i, cntx->tileProgress * 100.0);
            }
            printf("    \r");
            fflush(stdout);
            
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
