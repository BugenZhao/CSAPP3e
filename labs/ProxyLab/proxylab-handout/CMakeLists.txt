add_executable(bproxy proxy.c proxy.h rio_p.c)

add_custom_target(proxy_grade
        COMMAND cp proxy ${PROJECT_SOURCE_DIR}/labs/ProxyLab/proxylab-handout
        COMMAND cd ${PROJECT_SOURCE_DIR}/labs/ProxyLab/proxylab-handout && ./driver.sh
        DEPENDS proxy)