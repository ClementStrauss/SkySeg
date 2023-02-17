# SkySegmenatation

```
conda env create -f environment.yml
```

# Repository Structure :
## OO_Dataset
- Script to Download and convert coco panoptic to binary masks for the sky label. Only The Coco validation set is used as it is smaller in size.
- Notebook to load and display image from ADE20K


  
## AutoGrabCut_Cpp
- Computer vision based sky segmentation, run near real time on 720p video  
- Developped in C++, need to be compiled. Instructions included in folder    


### Paper of interest
https://github.com/xuebinqin/U-2-Net  
https://google.github.io/sky-optimization/  
https://github.com/xiongzhu666/Sky-Segmentation-and-Post-processing  

https://github.com/CSAILVision/semantic-segmentation-pytorch  
