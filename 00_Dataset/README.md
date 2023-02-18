
# COCO Dateset preparation

to download simply run :

```
chmod +x DownloadCoco.sh && ./DownloadCoco.sh
```

To extract binary masks on coco edit createBinaryMaskImagesFromCoco.py to point to your files then run the export script : 
```
createBinaryMaskImagesFromCoco.py
```

NOTE : **createBinaryMaskImagesFromCoco.py** does not rely on coco API and directly parse annotations and Jsons


# ADE20K Dateset preparation
a Notebook to load dataset, display image, and sky binary mask. 

Also contain code to export a dataset with binary mask image. Same as coco, edit the path before running.

