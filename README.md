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


Edit 02/21/2023 : something is broken in the last fastai version (2.7.11)

```
conda create --name fast python=3.9
conda activate fast
conda install -c fastchan fastai==2.7.10
conda install jupyterlab
pip install opencv-python
```

## 02_AutoGrabCut_Cpp
- Computer vision based sky segmentation, run near real time on 720p video  
- Developped in C++. It needs to be compiled. Instructions included in folder    

## TestImages, TestVideo
- images and video for subjective testing.
- Results on images are located in both **01_DeepLearning** and **02_AutoGrabCut_Cpp**
- Results on the video is located in **02_AutoGrabCut_Cpp**

## report.ipynb
- this is the notebook used to generate the report's web page, using quarto tool : https://quarto.org/
