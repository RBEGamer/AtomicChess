echo "-- BUILD FINISHED --"
# TODO ADD SECONDS .config only for base sytem
bash /var/buildroot/build.sh
# CREATE FOLDERS
mkdir -p /var/build_result/images
mkdir -p /var/build_result/host
# COPY BUILD RESULT TO HOST
cp -R /var/buildroot/output/images /var/build_result/images
cp -R /var/buildroot/output/host /var/build_result/host