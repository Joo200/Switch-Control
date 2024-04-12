# Switch Control

Software to use servos as switch motors and buttons to change direction.

Configurable with an embedded webserver and a small VueJS web app.

# Setup toolchains

This project requires esp-idf 5.2.2 and nodejs 20.

## Setup esp-idf

Follow [the instructions](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#installation)
to install and use esp-idf.

## Setup nodejs

Setup nodejs, make sure to install the version nodejs20 LTS. Other versions may not work.

Additionally to that npm is required to build the web application.

# Compile project

## Compile web application

First install all requirements. This has to be done once.

```shell
npm install
```

Build the web application. The output files can be found in `web/dist/`

```shell
npm run build
```

## Compile the project for esp

Either use the integration of your used IDE or run the command manually:

```shell
# Compile
idf.py -B cmake-build-target build
# Compile, flash and monitor
idf.py -B cmake-build-target build flash monitor
```

## Common issues

* Some esp-idf USB-UART adapters can be used by brltty. Either modify the udev rules or uninstall brltty.

# Used pins

In the default layout the following pins are used

| Channel | PIN | LEDC Timer   | LEDC Channel   | Purpose                       |
|---------|-----|--------------|----------------|-------------------------------|
| A1      | 25  | LEDC_TIMER_0 | LEDC_CHANNEL_0 | Servo Out, Smart Input Button |
| A2      | 13  | LEDC_TIMER_0 | LEDC_CHANNEL_1 | Servo Out, Smart Input Button |
| A3      | 23  | LEDC_TIMER_0 | LEDC_CHANNEL_2 | Servo Out, Smart Input Button |
| A4      | 19  | LEDC_TIMER_0 | LEDC_CHANNEL_3 | Servo Out, Smart Input Button | 
| A5      | 18  | LEDC_TIMER_0 | LEDC_CHANNEL_4 | Servo Out, Smart Input Button |
| A6      | 17  | LEDC_TIMER_0 | LEDC_CHANNEL_5 | Servo Out, Smart Input Button |
| A7      | 16  | LEDC_TIMER_0 | LEDC_CHANNEL_6 | Servo Out, Smart Input Button |
| A8      | 4   | LEDC_TIMER_0 | LEDC_CHANNEL_7 | Servo Out, Smart Input Button |
| B1      | 22  | -            | -              | Smart Input Button, I2C       |
| B2      | 21  | -            | -              | Smart Input Button, I2C       |
| B3      | 32  | -            | -              | Smart Input Button            |
| B4      | 33  | -            | -              | Smart Input Button            |
| B5      | 26  | -            | -              | Smart Input Button            |
| B6      | 27  | -            | -              | Smart Input Button            |
| B7      | 14  | -            | -              | Smart Input Button            |
| B8      | 15  | -            | -              | Smart Input Button            |
| X       | 0   | -            | -              | WiFi Toggle Button            |
| X       | 2   | -            | -              | Status LED                    | 

# Running Unit Tests

The project uses a combination of tests from esp and google test for unit tests.

With the following commands the unit tests can be executed:

```shell
idf.py --preview set-target linux build 
```
