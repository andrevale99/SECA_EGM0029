# Configuração no modo *station*

Exemplo de como conectar configurar o ESP32 no modo *station* para
acessar a rede.

## Lógica para configuração do ESP32 no modo *station*

1. Iniciar a partição NVS do esp com *nvs_flash_init()*;
2. Iniciar a pilha do TCP/IP *esp_netif_init()*;
3. Chamar a função *esp_event_loop_create_default()*;
4. Chamar a função *esp_netif_create_default_wifi_sta()*;

5. Inicialização do WIfi pela configuração padrão:
```
wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
ESP_ERROR_CHECK(esp_wifi_init(&cfg));
```
Caso deseja fazer uma configuração mais especifica, verificar as variáveis da estrutura *wifi_init_config_t*;

6. Cria um *handle* para os eventos de WIFi:
```
    esp_event_handler_instance_t instance_any_id;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
```

7. Configura a estrutura de dados, *wifi_config_t*, para realizar a conexão com a rede
com o ESP32 no modo *station*.

8. Por fim iniciar o ESP32 no modo *station*, colocar as configurações do *wifi_config_t*
e iniciar o WiFi:
```
ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
ESP_ERROR_CHECK(esp_wifi_start());
```

## *event_handler*

Nessa função é onde ocorre o tratamento dos eventos dispardos pelo WiFi. A função
verifica se foi um evento ocasionado pelo WiFi (*WIFI_EVENT*) e qual tipo de evento foi (*WIFI_EVENT_STA_XXX*):
```
    //Exemplo do código
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        ... //Tratamento
    }
```

Declaração da função de tratamendo dos eventos do WiFi:
```
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
```