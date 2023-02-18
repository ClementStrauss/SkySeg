

coco='/home/cstrauss/PERSO/progo/orpogSkySeg/00_Dataset/cocoSky'

outPutMaskFolder="cocoCppResults"

mkdir -p $outPutMaskFolder

find $coco -regex ".*\.\(jpg\)" > rgbImageList

for image in $(cat rgbImageList)
do
   echo "$image"
   base=$(basename $image)
   echo $base

   ./build/sky $image $outPutMaskFolder/$base

done

