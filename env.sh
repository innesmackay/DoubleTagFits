if hash foobar &> /dev/null; then
  deactivate
fi

ANALYSIS_NAME="DOUBLE_TAG_FITS"
user=$(whoami)

# General Library paths
export BASE_PATH=/home/${user}/DoubleTagFits/

# Check if conda is installed
# make noise if not
if [ "$(command -v conda)" = "" ]; then
    echo "conda is NOT installed. You need conda to run code in shell (see README.md)";
    echo "  -> ALTERNATIVE: execute analysis in DOCKER image following README.md";
    return 1;
fi
conda activate pid_env

# Run make
cd build
cmake ..
make install -j 16
cd -
