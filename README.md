#SkelGenerator
##Introduction
SkelGenerator is a application (and library) that allow to obtain tracing files (in SWC or ASC format) from the VRML 
files exported by Imaris Filament Tracer plugin.

In additon the tool can also obtain information about soma from traditional Imaris ans use them to generate a soma in
the output tracing.

##Dependencies
- VCGlib (dev branch)*
- Qt 5.10
- Eigen3
- Boost (with filesystem)
- MeshReconstructWrapper*

(*) This libs are downloaded and build automatically
##Building
SkelGenerator has been successfully built and used on Ubuntu 16.04 LTS, Mac OSX High Sierra and Windows 10 64-bits with
Visual Studio 2015. Please ensure that youbuild the Release version in order to get the best performance possible.

```
git clone https://github.com/vg-lab/SkelGenerator
mkdir SkelGenerator/build && cd SkelGenerator/build
git submodule update --init
cmake .. -DCLONE_SUBPROJECTS=ON
make
```