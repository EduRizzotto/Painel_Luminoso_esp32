# 💡 Painel Luminoso ESP32 — Rizzotto & Rizzotto

Sistema embarcado desenvolvido para o microcontrolador **ESP32** com servidor web integrado para controle remoto e automação dos painéis luminosos da **Rizzotto & Rizzotto**.

O projeto permite alternar entre **5 modos automáticos de animação** e **controle manual individual** em tempo real diretamente pelo navegador do celular ou computador na rede local, sem necessidade de aplicativos externos ou conexão com a internet.

---

## ✨ Funcionalidades

- **Servidor Web Embarcado (HTTP):** Interface web responsiva (mobile-first) servida diretamente pela memória do ESP32.
- **Imagens Embutidas em PROGMEM:** Logotipos armazenados em arrays binários (`PROGMEM`) no cabeçalho `imagens.h`, garantindo carregamento rápido sem depender de SPIFFS/LittleFS ou acesso externo.
- **Comunicação Assíncrona (Fetch API):** Troca de modos e acionamento das luzes sem recarregar a página.
- **Arquitetura Não-Bloqueante (`millis()`):** Todas as rotinas de temporização e efeitos visuais utilizam controle de tempo assíncrono com `millis()`, mantendo o servidor Wi-Fi responsivo o tempo todo.
- **Modo Manual Inteligente:** Ao tocar em qualquer um dos blocos luminosos na interface, o sistema entra automaticamente em **Modo Manual**, permitindo ligar/desligar cada painel individualmente. Ao clicar no botão de modos, o sistema retoma o ciclo de efeitos automáticos.

---

## 🎛️ Modos de Operação

| Modo | Descrição do Efeito |
| :--- | :--- |
| **Modo Manual** | Acionamento independente da Luz Superior e Luz Inferior pelo toque na interface. |
| **Modo 1** | Alternância contínua entre a luz superior e a luz inferior a cada `1000 ms` (1 segundo). |
| **Modo 2** | Sequência progressiva em 3 etapas de `1000 ms`: acende a 1ª luz ➔ acende a 2ª luz (ambas acesas) ➔ apaga ambas. |
| **Modo 3** | Pisca simultâneo rápido: ambas as luzes acendem por `500 ms` e apagam por `500 ms`. |
| **Modo 4** | Pisca simultâneo lento: ambas as luzes acendem por `2000 ms` e apagam por `2000 ms`. |
| **Modo 5** | Efeito estroboscópico de partida/desligamento: a luz superior dá 2 piscadas rápidas (`120 ms`) e acende; após um breve intervalo (`400 ms`), a luz inferior dá 2 piscadas rápidas e acende. Ambas permanecem acesas por `20 segundos` e desligam em sequência com 2 piscadas rápidas cada. |

---

## 🔌 Hardware e Pinagem

- **Microcontrolador:** ESP32 DOIT DevKit V1
- **Atuadores:** Módulo Relé de 4 canais (acionamento *Active LOW*: `LOW` liga, `HIGH` desliga)

| Componente / Canal | Pino ESP32 (GPIO) | Nível Ativo |
| :--- | :---: | :---: |
| **Luz Cima (Logo Quadrada)** | `GPIO 13` | `LOW` |
| **Luz Baixo (Banner Retangular)** | `GPIO 14` | `LOW` |
| **Alimentação Módulo Relé (VCC)** | `VIN (5V)` | — |
| **Terra (GND)** | `GND` | — |

> **Nota técnica:** O `GPIO 12` foi evitado propositalmente por ser um *strapping pin* do ESP32 (que altera a tensão da memória flash interna durante o boot caso seja puxado para nível alto pelo módulo relé).

---

## 🌐 Endpoints da API HTTP

O servidor web escuta na porta `80` e responde às seguintes rotas:

- `GET /` — Retorna a interface web completa (HTML5 + CSS3 + JS).
- `GET /mudarModo?modo=<1..5>` — Altera o modo automático ativo e reseta o estado manual.
- `GET /manual?c=<0|1>&b=<0|1>` — Ativa o **Modo Manual** (`modo = 0`) e define o estado imediato da luz de cima (`c`) e de baixo (`b`).
- `GET /logo.jpg` — Serve a imagem da logo principal a partir da memória Flash (`PROGMEM`).
- `GET /image-removebg-preview.png` — Serve a imagem secundária a partir da memória Flash (`PROGMEM`).

---

## 📁 Estrutura do Projeto

```text
Luz_esp32/
├── include/
│   └── imagens.h                  # Arrays binários (PROGMEM) das imagens servidas pelo ESP32
├── src/
│   ├── main.cpp                   # Código-fonte principal (Servidor Web, Wi-Fi e Máquina de Estados)
│   └── teste.txt                  # Rotina isolada para teste de bancada dos relés
├── index.html                     # Protótipo da interface web para testes locais no navegador
├── logo.jpg                       # Imagem da logo superior (quadrado)
├── image-removebg-preview.png     # Imagem da logo inferior (retângulo)
├── platformio.ini                 # Configurações de build, porta e velocidade do PlatformIO
└── README.md                      # Documentação do projeto
```

---

## 🚀 Como Compilar e Executar

### Pré-requisitos
- [Visual Studio Code](https://code.visualstudio.com/) com a extensão [PlatformIO IDE](https://platformio.org/) instalada.

### Passo a passo
1. Clone este repositório:
   ```bash
   git clone https://github.com/EduRizzotto/Painel_Luminoso_esp32.git
   cd Painel_Luminoso_esp32
   ```
2. Em `src/main.cpp`, verifique as credenciais da rede Wi-Fi (2.4 GHz):
   ```cpp
   WiFi.begin("SEU_WIFI", "SUA_SENHA");
   ```
3. Conecte o ESP32 via cabo USB, compile e envie o firmware pelo PlatformIO (**Upload**) ou via terminal:
   ```bash
   pio run --target upload
   ```
4. Abra o **Monitor Serial** (`115200 baud`) para visualizar o endereço IP atribuído ao ESP32:
   ```text
   ==================================
     CONECTADO AO WI-FI COM SUCESSO! 
     IP DO ESP32: 192.168.1.XXX
   ==================================
   ```
5. Acesse o endereço IP exibido diretamente no navegador do celular ou computador conectado à mesma rede.

---

## 👤 Autor

Desenvolvido por **Eduardo Rizzotto** para **Rizzotto & Rizzotto — Mecânica Agrícola / Mangueiras e Conexões Hidráulicas**.
