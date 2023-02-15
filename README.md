# SkySegmenatation

```
conda env create -f environment.yml
```

# Repo structure :
## OO_Dataset
Download and convert coco panoptic to binary masks for the sky label. 
The validation set from is used as it is smaller in size.

Images without sky are left out for now, can be added later 

simply run :

```
cd OO_Dataset
chmod +x DownloadCoco.sh && ./DownloadCoco.sh
python createBinaryMaskImagesFromCoco.py
```
  
## AutoGrabCut_Cpp
- Computer vision based sky segmentation, run near real time on 720p video  
- Developped in C++, need to be compiled. Instructions included in folder    


### Paper of interest
https://github.com/xuebinqin/U-2-Net  
https://google.github.io/sky-optimization/  
https://github.com/xiongzhu666/Sky-Segmentation-and-Post-processing  

https://github.com/CSAILVision/semantic-segmentation-pytorch  
