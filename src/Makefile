
FULL_OPTIMIZE = 1

ifdef FULL_OPTIMIZE
    CC = cc
    CXX = c++
    EXTRAFLAGS = -O3
else
    CC = cc
    CXX = c++
    EXTRAFLAGS = -g
endif

EXT_DIR = ext
INCLUDES = -I$(EXT_DIR) -I.

WFLAG = -Wall
WFLAG = 

CFLAGS = $(WFLAG) $(EXTRAFLAGS) $(INCLUDES)
CXXFLAGS = -std=c++11 -stdlib=libc++ $(WFLAG) $(EXTRAFLAGS) $(INCLUDES)
LDFLAGS = -lc++ $(EXTRAFLAGS)

OBJS = \
    main.o \
    aabb.o \
    abstracttonemapper.o \
    bvhnode.o \
    camera.o \
    commandqueue.o \
    eduptscene.o \
    framebuffer.o \
    intersection.o \
    renderer.o \
    scene.o \
    scenesupport.o \
    sceneobject.o \
    tonemapper.o \
    wavefrontobj.o \
    xmlscene.o \
    xmlobjloader.o \
    bsdfs/bsdf.o \
    bsdfs/lambert.o \
    bsdfs/burley.o \
    bsdfs/ggx.o \
    bsdfs/fullrefraction.o \
    bsdfs/fullreflection.o \
    bsdfs/translucent.o \
    materials/material.o \
    materials/fineglass.o \
    materials/lightmaterial.o \
    materials/singlebsdfmaterial.o \
    materials/skymaterial.o \
    materials/sppmaterial.o \
    geometries/aabbgeometry.o \
    geometries/sphere.o \
    geometries/mesh.o \
    textures/texture.o \
    textures/imagetexture.o \
    textures/noisetextures.o

EXT_OBJS = \
    $(EXT_DIR)/tinyxml/tinyxml2.o

#    $(EXT_DIR)/tinyexr/tinyexr.o \

MAINEXE = RrT
MAINTARGET = RrT

# main
$(MAINTARGET) : $(OBJS) $(EXT_OBJS)
	$(CC) -o "$(MAINEXE)" $(OBJS) $(EXT_OBJS) $(LDFLAGS)
	cp "$(MAINEXE)" ..

clean :
	rm -f $(OBJS) $(EXT_OBJS) "$(MAINEXE)"
