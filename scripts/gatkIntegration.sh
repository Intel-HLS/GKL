#!/usr/bin/env bash

read -t 5 -p "Enter Path to GKL/scripts: " GKL_SCRIPTS_PATH
export GKL_SCRIPTS_PATH=${GKL_SCRIPTS_PATH:-`pwd`}

#Check if gatk folder exists in GKL/scripts
if [ ! -d ${GKL_SCRIPTS_PATH}/gatk ]; then
        git clone https://github.com/broadinstitute/gatk.git
      fi
echo "Using ${GKL_SCRIPTS_PATH}/gatk"
export GKL_LOCAL_JAR=`ls ${GKL_SCRIPTS_PATH}/../build/libs/gkl-*jar`
echo $GKL_LOCAL_JAR

cd  ${GKL_SCRIPTS_PATH}/gatk

echo "Setting proxy"
PROXY_HOST=`echo $https_proxy | tr -s '/' ':' | cut -d ':' -f2`
PROXY_PORT=`echo $https_proxy | tr -s '/' ':' | cut -d ':' -f3`
export JAVA_PROXY="-Dhttps.proxyHost=$PROXY_HOST -Dhttps.proxyPort=$PROXY_PORT"

echo "Clean and Build"
git checkout ${GKL_SCRIPTS_PATH}/gatk/build.gradle
sed -i '361i    compile files("'$GKL_LOCAL_JAR'") ' ${GKL_SCRIPTS_PATH}/gatk/build.gradle
./gradlew $JAVA_PROXY clean
./gradlew $JAVA_PROXY localJar > build-gatk.out

echo "Run Test Cases"
./gradlew $JAVA_PROXY test --tests IntelInflaterDeflaterIntegrationTest
./gradlew $JAVA_PROXY test --tests HaplotypeUnitTest
./gradlew $JAVA_PROXY test --tests SmithWatermanIntelAlignerUnitTest


