# SkySegmenatation

Report is here : 
[report](report_pdf_export.pdf)


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

