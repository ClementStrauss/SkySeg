# SkySegmenatation

Report is here : 
[https://clementstrauss.github.io/SkySeg/](https://clementstrauss.github.io/SkySeg/)

# Repository Structure :

## 00_Dataset
- Script to Download and convert coco panoptic to binary masks for the sky label. Only The Coco validation set is used as it is smaller in size.
- Notebook to load and display image from ADE20K, convert it to binary image masks

## 01_DeepLearning
- checkout the train and evaluation notebook, and the report
- should run fine with conda : 

```
conda env create -f environment.yml
```

## 02_AutoGrabCut_Cpp
- Computer vision based sky segmentation, run near real time on 720p video  
- Developped in C++. It needs to be compiled. Instructions included in folder    

## TestImages, TestVideo
- images and video for subjective testing.
- Results on images are located in both **01_DeepLearning** and **02_AutoGrabCut_Cpp**
- Results on the video is located in **02_AutoGrabCut_Cpp**

## report.ipynb
- this is the notebook use to generate the report web page, using quarto
