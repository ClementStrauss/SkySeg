# SkySegmenatation

```
conda env create -f environment.yml
```

# Repo structure :
## OO_Dataset
Download and convert coco panoptic to binary masks for the sky label. 
The validation set is used as a dataset as it is smaller  
Images without sky are left out for now, can be added later 

simply run :

```
cd OO_Dataset
chmod +x DownloadCoco.sh && ./DownloadCoco.sh
python createBinaryMaskImagesFromCoco.py
```
  
