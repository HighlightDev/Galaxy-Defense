OPTSTRING=":amlst"
PATH_TO_ROOT=/home/dzinoviev/MyProjects/phengine/PHEngine
BUILD_PROJECT1=GalaxyDefense
BUILD_PROJECT2=EngineTest

while getopts ${OPTSTRING} opt; do
    case ${opt} in
    a)
        echo "Copy all resources directory."
        cp -a ${PATH_TO_ROOT}/PHEngine/res/. ${PATH_TO_ROOT}/build/${BUILD_PROJECT1}/res/
        cp -a ${PATH_TO_ROOT}/PHEngine/res/. ${PATH_TO_ROOT}/build/${BUILD_PROJECT2}/res/
        ;;
    m)
        echo "Copy models."
        cp -a ${PATH_TO_ROOT}/PHEngine/res/model/. ${PATH_TO_ROOT}/build/${BUILD_PROJECT1}/res/model/
        cp -a ${PATH_TO_ROOT}/PHEngine/res/model/. ${PATH_TO_ROOT}/build/${BUILD_PROJECT2}/res/model/
        ;;
    l)
        echo "Copy lua scripts."
        cp -a ${PATH_TO_ROOT}/PHEngine/res/scripts/. ${PATH_TO_ROOT}/build/${BUILD_PROJECT1}/res/scripts/
        cp -a ${PATH_TO_ROOT}/PHEngine/res/scripts/. ${PATH_TO_ROOT}/build/${BUILD_PROJECT2}/res/scripts/
        ;;
    s)
        echo "Copy shaders."
        cp -a ${PATH_TO_ROOT}/PHEngine/res/shaders/. ${PATH_TO_ROOT}/build/${BUILD_PROJECT1}/res/shaders/
        cp -a ${PATH_TO_ROOT}/PHEngine/res/shaders/. ${PATH_TO_ROOT}/build/${BUILD_PROJECT2}/res/shaders/
        ;;
    t)
        echo "Copy textures."
        cp -a ${PATH_TO_ROOT}/PHEngine/res/texture/. ${PATH_TO_ROOT}/build/${BUILD_PROJECT1}/res/texture/
        cp -a ${PATH_TO_ROOT}/PHEngine/res/texture/. ${PATH_TO_ROOT}/build/${BUILD_PROJECT2}/res/texture/
        ;;
    ?)
        echo "Invalid option: -${OPTARG}."
        exit 1
        ;;
    esac
done
