 install_dep(){
 curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.rpm.sh | sudo bash
 sudo yum install -y git-lfs
 git lfs install
 sudo yum install -y java-1.8.0-openjdk-devel git cmake patch libtool automake yasm zlib-devel centos-release-scl
 sudo yum install -y devtoolset-7-gcc-*
}
echo "Installing dependencies"
install_dep
source scl_source enable devtoolset-7
# export JAVA_PROXY="-Dhttp.proxyHost=proxy.example.com -Dhttp.proxyPort=1234 -Dhttps.proxyHost=proxy.example.com -Dhttps.proxyPort=1234"
# gcc --version
./gradlew $JAVA_PROXY clean
./gradlew $JAVA_PROXY build 
