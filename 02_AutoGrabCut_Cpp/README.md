## GrabCut base sky segmentation 

### installation & requirements
- requires opencv to be installed, C++ compiler, cmake

### Build
```
mkdir build && cd build
cmake ..
make
```

### Run in interactive mode
```
./sky YourVideoFile
```
or

```
./sky YourImage.{png, jpg, jpeg}
```


### Run in batch mode (save result as image or video)
```
./sky YourVideoFile OutoutVideoName
```
or

```
./sky YourImage.{png, jpg, jpeg} OutoutImageName
```
