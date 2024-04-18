OPTSTRING=":amlst"
PATH_TO_ROOT=/home/dzinoviev/MyProjects/phengine/PHEngine

while getopts ${OPTSTRING} opt; do
    case ${opt} in
    a)
        echo "Copy all resources directory."
        cp -a ${PATH_TO_ROOT}/PHEngine/res/. ${PATH_TO_ROOT}/build/GalaxyDefense/res/
        ;;
    m)
        echo "Copy models."
        cp -a ${PATH_TO_ROOT}/PHEngine/res/model/. ${PATH_TO_ROOT}/build/GalaxyDefense/res/model/
        ;;
    l)
        echo "Copy lua scripts."
        cp -a ${PATH_TO_ROOT}/PHEngine/res/scripts/. ${PATH_TO_ROOT}/build/GalaxyDefense/res/scripts/
        ;;
    s)
        echo "Copy shaders."
        cp -a ${PATH_TO_ROOT}/PHEngine/res/shaders/. ${PATH_TO_ROOT}/build/GalaxyDefense/res/shaders/
        ;;
    t)
        echo "Copy textures."
        cp -a ${PATH_TO_ROOT}/PHEngine/res/texture/. ${PATH_TO_ROOT}/build/GalaxyDefense/res/texture/
        ;;
    ?)
        echo "Invalid option: -${OPTARG}."
        exit 1
        ;;
    esac
done
