# 📡 Projeto ESP32 AI-Thinker – Sistema de Identificação e Comunicação IoT

## 📌 Visão Geral

Este projeto tem como objetivo o desenvolvimento de um **sistema embarcado baseado no ESP32 AI-Thinker**, voltado para aplicações de **IoT, automação e controle de acesso**, utilizando **RFID**, **sinalização por LED** e **comunicação via MQTT**.

A arquitetura foi pensada de forma **modular**, facilitando manutenção, reutilização de código e futuras expansões, como integração com dashboards, servidores em nuvem ou sistemas industriais.

---

## ⚙️ Funcionalidades Principais

* 📶 **Conectividade Wi‑Fi** utilizando o ESP32
* 🪪 **Leitura de tags RFID** para identificação de usuários ou objetos
* 💡 **Sinalização visual por LEDs** (feedback de estados como acesso permitido/negado, conexão, erro, etc.)
* ☁️ **Comunicação MQTT** para envio e recebimento de dados em tempo real
* 🔐 **Gerenciamento seguro de credenciais** (Wi‑Fi e MQTT)
* 🧩 **Estrutura modular em headers (.h)**, facilitando organização e escalabilidade

---

## 🧠 Arquitetura do Projeto

O projeto é dividido em módulos independentes, cada um responsável por uma função específica:

* **RFID**: leitura e tratamento das tags de identificação
* **MQTT**: comunicação com broker para publicação e assinatura de tópicos
* **LEDs**: controle de sinalização visual do sistema
* **Configurações sensíveis**: separação de credenciais para maior segurança

Essa abordagem permite:

* Fácil substituição de hardware
* Evolução do projeto sem impacto em todo o código
* Maior clareza e organização

---

## 🧪 Aplicações Possíveis

* Controle de acesso físico (portas, salas, laboratórios)
* Rastreamento e identificação de objetos
* Sistemas de presença ou autenticação
* Integração com plataformas IoT (Node‑RED, Home Assistant, dashboards próprios)
* Ambientes industriais ou educacionais

---

## 🚀 Objetivo do Repositório

Este repositório serve como:

* Base para estudos e experimentação com ESP32
* Exemplo prático de integração RFID + MQTT
* Projeto inicial para soluções IoT mais complexas

O código foi escrito com foco em **clareza, organização e expansão futura**.

---

## 🛠️ Tecnologias Utilizadas

* ESP32 AI‑Thinker
* Arduino Framework
* Protocolo MQTT
* RFID (MFRC522 ou similar)
* Wi‑Fi

---

📌 *Sinta‑se à vontade para clonar, adaptar e evoluir este projeto conforme sua necessidade.*
