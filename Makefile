SHELL=/bin/csh
SRC_VER = 2.0.1

TAGINFO = $(shell (echo -n "Compiled on:" `date`"\nby:" `whoami`@`hostname`"\n$(SESI_TAGINFO)") | sesitag -m)
CFLAGS := $(CFLAGS) ${TAGINFO} -DHOUDINI_MAJOR_RELEASE=${HOUDINI_MAJOR_RELEASE} -DHOUDINI_MINOR_RELEASE=${HOUDINI_MINOR_RELEASE} -DHOUDINI_BUILD_VERSION=${HOUDINI_BUILD_VERSION}
OPTIMIZER = -O2 $(CFLAGS)

real_flow_lib = real_flow_part.h real_flow_part.C real_flow_mesh.h real_flow_mesh.C real_flow_sd.h real_flow_sd.C real_flow_RWC.h real_flow_RWC.C

export_src = SOP_RF_Export.C SOP_RF_Export.h \
SOP_RF_Export_writeRFParticleFile.C \
SOP_RF_Export_writeSDFile.C \
SOP_RF_Export_writeSDFileRestGeo.C  \
SOP_RF_Export_writeSDFileAnimGeo.C \
SOP_RF_Import_ReadRFRWCFile.C \
real_flow_part.h real_flow_part.C \
real_flow_mesh.h \
real_flow_mesh.C \
real_flow_sd.h real_flow_sd.C \
real_flow_RWC.h real_flow_RWC.C

import_src = SOP_RF_Import.C SOP_RF_Import.h \
SOP_RF_Import_ReadRFParticleFile.C \
SOP_RF_Import_ReadRFMeshFile.C \
SOP_RF_Import_ReadRFSDFile.C \
SOP_RF_Import_ReadRFSDFileRestGeo.C SOP_RF_Import_ReadRFSDFileAnimGeo.C \
SOP_RF_Export_writeRWCFile.C \
real_flow_part.h real_flow_part.C \
real_flow_mesh.h real_flow_mesh.C \
real_flow_sd.h real_flow_sd.C \
real_flow_RWC.h real_flow_RWC.C

all: SOP_RF_Import SOP_RF_Export dump_rf_part dump_rf_mesh dump_rf_sd dump_rf_rwc

dump_rf_part: dump_rf_part.C real_flow_part.h real_flow_part.C
	g++ -g dump_rf_part.C  -o dump_rf_part
	cp dump_rf_part ${HOME}/bin

dump_rf_mesh: dump_rf_mesh.C real_flow_mesh.h real_flow_mesh.C
	g++ -g dump_rf_mesh.C  -o dump_rf_mesh
	cp dump_rf_mesh ${HOME}/bin

dump_rf_sd: dump_rf_sd.C real_flow_sd.h real_flow_sd.C
	g++ -g dump_rf_sd.C  -o dump_rf_sd
	cp dump_rf_sd ${HOME}/bin

dump_rf_rwc: dump_rf_rwc.C real_flow_RWC.h real_flow_RWC.C
	g++ -g dump_rf_rwc.C  -o dump_rf_rwc
	cp dump_rf_rwc ${HOME}/bin


SOP_RF_Import : $(import_src)
	g++ -g $(CFLAGS) -DVERSION=\"${VERSION}\" -DDLLEXPORT=  -D_GNU_SOURCE -DLINUX -DAMD64 -m64 -fPIC -DSIZEOF_VOID_P=8 -DSESI_LITTLE_ENDIAN -DENABLE_THREADS -DUSE_PTHREADS -D_REENTRANT -D_FILE_OFFSET_BITS=64 -c  -DGCC4 -DGCC3 -Wno-deprecated -I/opt/hfs${HOUDINI_VERSION}/toolkit/include -I/opt/hfs${HOUDINI_VERSION}/toolkit/include/htools -Wall -W -Wno-parentheses -Wno-sign-compare -Wno-reorder -Wno-uninitialized -Wunused -Wno-unused-parameter -O2 -DMAKING_DSO -o SOP_RF_Import.o SOP_RF_Import.C
	g++ -shared SOP_RF_Import.o -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lGLU -lGL -lX11 -lXext -lXi -ldl -o ./SOP_RF_Import.so

SOP_RF_Export : $(export_src)
	g++ -g $(CFLAGS) -DVERSION=\"${VERSION}\" -DDLLEXPORT=  -D_GNU_SOURCE -DLINUX -DAMD64 -m64 -fPIC -DSIZEOF_VOID_P=8 -DSESI_LITTLE_ENDIAN -DENABLE_THREADS -DUSE_PTHREADS -D_REENTRANT -D_FILE_OFFSET_BITS=64 -c  -DGCC4 -DGCC3 -Wno-deprecated -I/opt/hfs${HOUDINI_VERSION}/toolkit/include -I/opt/hfs${HOUDINI_VERSION}/toolkit/include/htools -Wall -W -Wno-parentheses -Wno-sign-compare -Wno-reorder -Wno-uninitialized -Wunused -Wno-unused-parameter -O2 -DMAKING_DSO -o SOP_RF_Export.o SOP_RF_Export.C
	g++ -shared SOP_RF_Export.o -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lGLU -lGL -lX11 -lXext -lXi -ldl -o ./SOP_RF_Export.so

archive_src:
	rm -fr html
	doxygen Doxyfile
	tar zcvf Houdini.RealFlow_v${SRC_VER}.src.tar.gz \
		$(import_src) $(export_src) \
		$(real_flow_lib) \
		dump_rf_sd.C dump_rf_part.C dump_rf_mesh.C dump_rf_rwc.C \
		html/	\
		Makefile Doxyfile \
		README.txt \
		rf_import.txt  rf_export.txt

clean:
	rm -f *.o dump_rf_part dump_rf_mesh dump_rf_sd dump_rf_rwc *.so *.dll *.dylib


