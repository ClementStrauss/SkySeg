

folder='../TestImages'

outPutMaskFolder="TestImagesCppResults"

mkdir -p $outPutMaskFolder

rm rgbImageList

find $folder -regex ".*\.\(jpg\|png\|jpeg\)" > rgbImageList

for image in $(cat rgbImageList)
do
   echo "$image"
   base=$(basename $image)
   echo $base

   ./build/sky $image $outPutMaskFolder/$base

done

