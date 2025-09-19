idf_component_register(SRCS
    $<$<BOOL:${CONFIG_USE_FMA_NIMBLE_SERVER}>:src/fma_nimble_server.c>
    $<$<BOOL:${CONFIG_USE_FMA_NIMBLE_SERVER}>:src/gap.c>
    $<$<BOOL:${CONFIG_USE_FMA_NIMBLE_SERVER}>:src/gatt_svc.c>
    
    INCLUDE_DIRS "." "include"
    PRIV_REQUIRES bt nvs_flash

)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/bluetooth/common)
