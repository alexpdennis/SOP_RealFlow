SHELL=/bin/csh
SRC_VER = 2.0.1

TAGINFO = $(shell (echo -n "Compiled on:" `date`"\nby:" `whoami`@`hostname`"\n$(SESI_TAGINFO)") | sesitag -m)
H_CFLAGS =  $(shell hcustom --cflags)
CFLAGS := $(CFLAGS) $(H_CFLAGS) -DMAKING_DSO $(TAGINFO)

real_flow_lib = real_flow_part.h real_flow_part.C real_flow_mesh.h real_flow_mesh.C real_flow_sd.h real_flow_sd.C real_flow_RWC.h real_flow_RWC.C

export_src = SOP_RF_Export.C SOP_RF_Export.h \
SOP_RF_Export_writeRFParticleFile.C \
SOP_RF_Export_writeSDFile.C \
SOP_RF_Export_writeSDFileRestGeo.C  \
SOP_RF_Export_writeSDFileAnimGeo.C \
SOP_RF_Export_writeRWCFile.C \
SOP_RF_Export_writeMeshFile.C \
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
SOP_RF_Import_ReadRFRWCFile.C \
real_flow_part.h real_flow_part.C \
real_flow_mesh.h real_flow_mesh.C \
real_flow_sd.h real_flow_sd.C \
real_flow_RWC.h real_flow_RWC.C

all: SOP_RF_Import SOP_RF_Export dump_rf_part dump_rf_mesh dump_rf_sd dump_rf_rwc
DebugRFExport:  SOP_RF_Export install
ReleaseRFExport: SOP_RF_Export install
DebugRFImport:  SOP_RF_Import install
ReleaseRFImport: SOP_RF_Import install


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


SOP_RF_Import :
	g++ -g $(CFLAGS) SOP_RF_Import.C
	g++ -shared SOP_RF_Import.o -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lGLU -lGL -lX11 -lXext -lXi -ldl -o ./SOP_RF_Import.so

SOP_RF_Export :
	g++ -g $(CFLAGS) SOP_RF_Export.C
	g++ -shared SOP_RF_Export.o -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lGLU -lGL -lX11 -lXext -lXi -ldl -o ./SOP_RF_Export.so


archive_src:
	rm -fr html
	doxygen Doxyfile
	tar zcvf Houdini.RealFlow_v${SRC_VER}.src.tar.gz \
		$(import_src) $(export_src) \
		$(real_flow_lib) \
		dump_rf_sd.C dump_rf_part.C dump_rf_mesh.C dump_rf_rwc.C \
		docs/	\
		Makefile Doxyfile \
		README.txt 

install:
	cp SOP_RF_Import.so $(DCA_COMMON)/lib/houdini/dso_x86_64/
	cp SOP_RF_Export.so $(DCA_COMMON)/lib/houdini/dso_x86_64/

clean:
	rm -f *.o dump_rf_part dump_rf_mesh dump_rf_sd dump_rf_rwc *.so *.dylib
