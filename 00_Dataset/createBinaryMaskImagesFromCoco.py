#!/usr/bin/env python3
'''
Convert COCO panotic annotation to binary mask

based on https://github.com/cocodataset/panopticapi (from visualization.py)

'''

import os
import sys
import numpy as np
import json

import PIL.Image as Image
import matplotlib.pyplot as plt
import cv2


def rgb2id(color):
    if isinstance(color, np.ndarray) and len(color.shape) == 3:
        if color.dtype == np.uint8:
            color = color.astype(np.int32)
        return color[:, :, 0] + 256 * color[:, :, 1] + 256 * 256 * color[:, :, 2]
    return int(color[0] + 256 * color[1] + 256 * 256 * color[2])


json_file = '/home/cstrauss/PERSO/progo/coco/panoptic_annotations_trainval2017/annotations/panoptic_val2017.json'
segmentations_folder = '/home/cstrauss/PERSO/progo/coco/panoptic_annotations_trainval2017/annotations/panoptic_val2017/panoptic_val2017'
img_folder = '/home/cstrauss/PERSO/progo/coco/images/val2017'
binary_mask_output_folder = './cocoSky'
coco_sky_class_id = 187

try:
    os.mkdir(binary_mask_output_folder)
except OSError as error:
    print(error)    

with open(json_file, 'r') as f:
    coco_d = json.load(f)


# print(coco_d['annotations'])
#ann = np.random.choice(coco_d['annotations'])

def save_binary_maks(ann, keep_only_images_with_sky=True):

    if keep_only_images_with_sky:
        has_sky = False
        for segment_info in ann['segments_info']:
            if segment_info['category_id'] == coco_sky_class_id:
                has_sky = True
                break

        if not has_sky:
            return

    # find input img that correspond to the annotation
    img = None
    image_name = ""
    for image_info in coco_d['images']:
        if image_info['id'] == ann['image_id']:
            try:
                image_name = image_info['file_name']
                # img = np.array(
                #     Image.open(os.path.join(img_folder, image_name))
                # )
                try:
                    # simlink rgb image into the same folder, eveythings in one place
                    os.symlink(os.path.join(img_folder, image_name), os.path.join(binary_mask_output_folder, image_name))
                except OSError as error:
                    print(error)   
            except:
                print("Undable to find correspoding input image.")
            break

    segmentation = np.array(
        Image.open(os.path.join(segmentations_folder, ann['file_name'])),
        dtype=np.uint8
    )

    # create a blank binary mask (uint8 grayscale image)
    maskImg = np.zeros(shape=(segmentation.shape[0], segmentation.shape[1]), dtype=np.uint8)
    
    segmentation_id = rgb2id(segmentation) # label map

    for segment_info in ann['segments_info']:
        if segment_info['category_id'] == coco_sky_class_id:
            # color = color_generator.get_color(segment_info['category_id'])
            mask = segmentation_id == segment_info['id']
            maskImg[mask] = 255

    imagePath = binary_mask_output_folder + '/' + image_name + '_skyMask.png'
    cv2.imwrite(imagePath, maskImg)
    print(" saved " + imagePath)

# ann = coco_d['annotations'][3]
# save_binary_maks(ann)


for annotation in coco_d['annotations']:
    save_binary_maks(annotation)
