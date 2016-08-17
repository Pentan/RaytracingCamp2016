
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

CFLAGS = -Wall $(EXTRAFLAGS) $(INCLUDES)
CXXFLAGS = -std=c++11 -stdlib=libc++ -Wall $(EXTRAFLAGS) $(INCLUDES)
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
    bsdfs/lambert.o \
    bsdfs/fullrefraction.o \
    bsdfs/fullreflection.o \
    bsdfs/paint.o \
    bsdfs/translucent.o \
    materials/singlebsdfmaterial.o \
    materials/simplesky.o \
    geometries/aabbgeometry.o \
    geometries/sphere.o \
    geometries/mesh.o \
    texture/texture.o \
    texture/imagetexture.o \
    texture/noisetexture.o

EXT_OBJS = \
    $(EXT_DIR)/tinyxml/tinyxml2.o

#    $(EXT_DIR)/tinyexr/tinyexr.o \

MAINEXE = r5m
MAINTARGET = r5m

# main
$(MAINTARGET) : $(OBJS) $(EXT_OBJS)
	$(CC) -o "$(MAINEXE)" $(OBJS) $(EXT_OBJS) $(LDFLAGS)

clean :
	rm -f $(OBJS) $(EXT_OBJS) "$(MAINEXE)"
