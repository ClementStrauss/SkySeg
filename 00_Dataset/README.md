
# COCO Dateset preparation

simply run :

```
chmod +x DownloadCoco.sh && ./DownloadCoco.sh
python createBinaryMaskImagesFromCoco.py
```

NOTE : **createBinaryMaskImagesFromCoco.py** does not rely on coco API and directly parse annotations and Jsons

# ADE20K

a demo Notebook to load and display image, and sky binary mask. 
