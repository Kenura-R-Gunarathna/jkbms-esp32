#ifndef STRUCTS_H
#define STRUCTS_H

#include <Arduino.h>
#include <map>

// Definition of DataFieldConfig structure
struct DataFieldConfig {
    String name;
    String code;
    String description;
    int index;
    int data_length;
    String label;
    String unit;
    String type;
    float scale;
    bool signed_value;
    struct {
        int cell_index_base;
        int data_length;
        String name;
        String unit;
        String type;
        float scale;
    } cell;
    int cellcount_byte_index;
    int cell_count_multiplier;
};

// Definition of BMSConfig structure
struct BMSConfig {
    struct {
        String start_bytes[2];
        int length_byte_index;
    } header;
    std::map<String, DataFieldConfig> dataFields;
};


BMSConfig initializeConfig() {
    BMSConfig config;

    // --- Header ---
    config.header.start_bytes[0] = "0x4E";
    config.header.start_bytes[1] = "0x57";
    config.header.length_byte_index = 3;

    // --- Data Fields ---
    DataFieldConfig cellVoltagesConfig;
    cellVoltagesConfig.name = "cell_voltages";
    cellVoltagesConfig.code = "0x79";
    cellVoltagesConfig.description = "Individual Cell Voltage";
    cellVoltagesConfig.cellcount_byte_index = 16;
    cellVoltagesConfig.cell_count_multiplier = 3;
    cellVoltagesConfig.cell.cell_index_base = 17;
    cellVoltagesConfig.cell.data_length = 3;
    cellVoltagesConfig.cell.name = "Cell Voltage";
    cellVoltagesConfig.cell.unit = "V";
    cellVoltagesConfig.cell.type = "float";
    cellVoltagesConfig.cell.scale = 0.001;
    config.dataFields["cell_voltages"] = cellVoltagesConfig;

    DataFieldConfig powerTubeTempConfig;
    powerTubeTempConfig.name = "power_tube_temperature";
    powerTubeTempConfig.code = "0x80";
    powerTubeTempConfig.description = "Power Tube Temperature";
    powerTubeTempConfig.index = 40;
    powerTubeTempConfig.data_length = 1;
    powerTubeTempConfig.label = "Power Tube Temp";
    powerTubeTempConfig.unit = "°C";
    powerTubeTempConfig.type = "int";
    powerTubeTempConfig.scale = 1.0;
    powerTubeTempConfig.signed_value = false;
    config.dataFields["power_tube_temperature"] = powerTubeTempConfig;

    DataFieldConfig battery_box_temperature;
    battery_box_temperature.name = "battery_box_temperature";
    battery_box_temperature.code = "0x81";
    battery_box_temperature.description = "Battery Box Temperature";
    battery_box_temperature.index = 42;
    battery_box_temperature.data_length = 1;
    battery_box_temperature.label = "Battery Box Temp";
    battery_box_temperature.unit = "°C";
    battery_box_temperature.type = "int";
    battery_box_temperature.scale = 1.0;
    battery_box_temperature.signed_value = false;
    config.dataFields["battery_box_temperature"] = battery_box_temperature;

    DataFieldConfig battery_temperature;
    battery_temperature.name = "battery_temperature";
    battery_temperature.code = "0x82";
    battery_temperature.description = "Battery Temperature";
    battery_temperature.index = 44;
    battery_temperature.data_length = 1;
    battery_temperature.label = "Battery Temp";
    battery_temperature.unit = "°C";
    battery_temperature.type = "int";
    battery_temperature.scale = 1.0;
    battery_temperature.signed_value = false;
    config.dataFields["battery_temperature"] = battery_temperature;

    DataFieldConfig total_battery_voltage;
    total_battery_voltage.name = "total_battery_voltage";
    total_battery_voltage.code = "0x83";
    total_battery_voltage.description = "Total Battery Voltage";
    total_battery_voltage.index = 46;
    total_battery_voltage.data_length = 2;
    total_battery_voltage.label = "Total Voltage";
    total_battery_voltage.unit = "V";
    total_battery_voltage.type = "float";
    total_battery_voltage.scale = 0.01;
    total_battery_voltage.signed_value = false;
    config.dataFields["total_battery_voltage"] = total_battery_voltage;

    DataFieldConfig current_data;
    current_data.name = "current_data";
    current_data.code = "0x84";
    current_data.description = "Current Data";
    current_data.index = 48;
    current_data.data_length = 2;
    current_data.label = "Current";
    current_data.unit = "A";
    current_data.type = "float";
    current_data.scale = 0.01;
    current_data.signed_value = true;
    config.dataFields["current_data"] = current_data;

    DataFieldConfig soc;
    soc.name = "soc";
    soc.code = "0x85";
    soc.description = "State of Charge";
    soc.index = 50;
    soc.data_length = 1;
    soc.label = "SOC";
    soc.unit = "%";
    soc.type = "int";
    soc.scale = 1.0;
    soc.signed_value = false;
    config.dataFields["soc"] = soc;

    DataFieldConfig number_of_battery_temperature_sensors;
    number_of_battery_temperature_sensors.name = "number_of_battery_temperature_sensors";
    number_of_battery_temperature_sensors.code = "0x86";
    number_of_battery_temperature_sensors.description = "Number of Battery Temperature Sensors";
    number_of_battery_temperature_sensors.index = 51;
    number_of_battery_temperature_sensors.data_length = 1;
    number_of_battery_temperature_sensors.label = "Number of Battery Temp Sensors";
    number_of_battery_temperature_sensors.type = "int";
    number_of_battery_temperature_sensors.scale = 1.0;
    number_of_battery_temperature_sensors.signed_value = false;
    config.dataFields["number_of_battery_temperature_sensors"] = number_of_battery_temperature_sensors;

    DataFieldConfig number_of_battery_cycles;
    number_of_battery_cycles.name = "number_of_battery_cycles";
    number_of_battery_cycles.code = "0x87";
    number_of_battery_cycles.description = "Number of Battery Cycles";
    number_of_battery_cycles.index = 52;
    number_of_battery_cycles.data_length = 2;
    number_of_battery_cycles.label = "Number of Battery Cycles";
    number_of_battery_cycles.type = "int";
    number_of_battery_cycles.scale = 1.0;
    number_of_battery_cycles.signed_value = false;
    config.dataFields["number_of_battery_cycles"] = number_of_battery_cycles;

    DataFieldConfig total_battery_cycle_capacity;
    total_battery_cycle_capacity.name = "total_battery_cycle_capacity";
    total_battery_cycle_capacity.code = "0x89";
    total_battery_cycle_capacity.description = "Total Battery Cycle Capacity";
    total_battery_cycle_capacity.index = 54;
    total_battery_cycle_capacity.data_length = 4;
    total_battery_cycle_capacity.label = "Total Battery Cycle Capacity";
    total_battery_cycle_capacity.unit = "Ah";
    total_battery_cycle_capacity.type = "float";
    total_battery_cycle_capacity.scale = 1.0;
    total_battery_cycle_capacity.signed_value = false;
    config.dataFields["total_battery_cycle_capacity"] = total_battery_cycle_capacity;

    DataFieldConfig battery_strings;
    battery_strings.name = "battery_strings";
    battery_strings.code = "0x8A";
    battery_strings.description = "Number of Battery Strings";
    battery_strings.index = 58;
    battery_strings.data_length = 1;
    battery_strings.label = "Battery Strings";
    battery_strings.type = "int";
    battery_strings.scale = 1.0;
    battery_strings.signed_value = false;
    config.dataFields["battery_strings"] = battery_strings;

    DataFieldConfig battery_warning_message;
    battery_warning_message.name = "battery_warning_message";
    battery_warning_message.code = "0x8B";
    battery_warning_message.description = "Battery Warning Message";
    battery_warning_message.index = 59;
    battery_warning_message.data_length = 2;
    battery_warning_message.label = "Battery Warning Message";
    battery_warning_message.type = "int";
    battery_warning_message.scale = 1.0;
    battery_warning_message.signed_value = false;
    config.dataFields["battery_warning_message"] = battery_warning_message;

    DataFieldConfig battery_status_information;
    battery_status_information.name = "battery_status_information";
    battery_status_information.code = "0x8C";
    battery_status_information.description = "Battery Status Information";
    battery_status_information.index = 61;
    battery_status_information.data_length = 2;
    battery_status_information.label = "Battery Status Information";
    battery_status_information.type = "int";
    battery_status_information.scale = 1.0;
    battery_status_information.signed_value = false;
    config.dataFields["battery_status_information"] = battery_status_information;

    DataFieldConfig total_voltage_overvoltage_protection;
    total_voltage_overvoltage_protection.name = "total_voltage_overvoltage_protection";
    total_voltage_overvoltage_protection.code = "0x8E";
    total_voltage_overvoltage_protection.description = "Total Voltage Overvoltage Protection";
    total_voltage_overvoltage_protection.index = 63;
    total_voltage_overvoltage_protection.data_length = 2;
    total_voltage_overvoltage_protection.label = "Total Voltage Overvoltage Protection";
    total_voltage_overvoltage_protection.unit = "V";
    total_voltage_overvoltage_protection.type = "float";
    total_voltage_overvoltage_protection.scale = 0.01;
    total_voltage_overvoltage_protection.signed_value = false;
    config.dataFields["total_voltage_overvoltage_protection"] = total_voltage_overvoltage_protection;

    DataFieldConfig total_voltage_undervoltage_protection;
    total_voltage_undervoltage_protection.name = "total_voltage_undervoltage_protection";
    total_voltage_undervoltage_protection.code = "0x8F";
    total_voltage_undervoltage_protection.description = "Total Voltage Undervoltage Protection";
    total_voltage_undervoltage_protection.index = 65;
    total_voltage_undervoltage_protection.data_length = 2;
    total_voltage_undervoltage_protection.label = "Total Voltage Undervoltage Protection";
    total_voltage_undervoltage_protection.unit = "V";
    total_voltage_undervoltage_protection.type = "float";
    total_voltage_undervoltage_protection.scale = 0.01;
    total_voltage_undervoltage_protection.signed_value = false;
    config.dataFields["total_voltage_undervoltage_protection"] = total_voltage_undervoltage_protection;

    DataFieldConfig single_overvoltage_protection_voltage;
    single_overvoltage_protection_voltage.name = "single_overvoltage_protection_voltage";
    single_overvoltage_protection_voltage.code = "0x90";
    single_overvoltage_protection_voltage.description = "Single Overvoltage Protection Voltage";
    single_overvoltage_protection_voltage.index = 67;
    single_overvoltage_protection_voltage.data_length = 2;
    single_overvoltage_protection_voltage.label = "Single Overvoltage Protection Voltage";
    single_overvoltage_protection_voltage.unit = "mV";
    single_overvoltage_protection_voltage.type = "int";
    single_overvoltage_protection_voltage.scale = 1.0;
    single_overvoltage_protection_voltage.signed_value = false;
    config.dataFields["single_overvoltage_protection_voltage"] = single_overvoltage_protection_voltage;

    DataFieldConfig cell_overvoltage_recovery_voltage;
    cell_overvoltage_recovery_voltage.name = "cell_overvoltage_recovery_voltage";
    cell_overvoltage_recovery_voltage.code = "0x91";
    cell_overvoltage_recovery_voltage.description = "Cell Overvoltage Recovery Voltage";
    cell_overvoltage_recovery_voltage.index = 69;
    cell_overvoltage_recovery_voltage.data_length = 2;
    cell_overvoltage_recovery_voltage.label = "Cell Overvoltage Recovery Voltage";
    cell_overvoltage_recovery_voltage.unit = "mV";
    cell_overvoltage_recovery_voltage.type = "int";
    cell_overvoltage_recovery_voltage.scale = 1.0;
    cell_overvoltage_recovery_voltage.signed_value = false;
    config.dataFields["cell_overvoltage_recovery_voltage"] = cell_overvoltage_recovery_voltage;

    DataFieldConfig single_overvoltage_protection_delay;
    single_overvoltage_protection_delay.name = "single_overvoltage_protection_delay";
    single_overvoltage_protection_delay.code = "0x92";
    single_overvoltage_protection_delay.description = "Single Overvoltage Protection Delay";
    single_overvoltage_protection_delay.index = 71;
    single_overvoltage_protection_delay.data_length = 2;
    single_overvoltage_protection_delay.label = "Single Overvoltage Protection Delay";
    single_overvoltage_protection_delay.unit = "seconds";
    single_overvoltage_protection_delay.type = "int";
    single_overvoltage_protection_delay.scale = 1.0;
    single_overvoltage_protection_delay.signed_value = false;
    config.dataFields["single_overvoltage_protection_delay"] = single_overvoltage_protection_delay;

    DataFieldConfig single_undervoltage_protection_voltage;
    single_undervoltage_protection_voltage.name = "single_undervoltage_protection_voltage";
    single_undervoltage_protection_voltage.code = "0x93";
    single_undervoltage_protection_voltage.description = "Single Undervoltage Protection Voltage";
    single_undervoltage_protection_voltage.index = 73;
    single_undervoltage_protection_voltage.data_length = 2;
    single_undervoltage_protection_voltage.label = "Single Undervoltage Protection Voltage";
    single_undervoltage_protection_voltage.unit = "mV";
    single_undervoltage_protection_voltage.type = "int";
    single_undervoltage_protection_voltage.scale = 1.0;
    single_undervoltage_protection_voltage.signed_value = false;
    config.dataFields["single_undervoltage_protection_voltage"] = single_undervoltage_protection_voltage;

    DataFieldConfig monomer_undervoltage_recovery_voltage;
    monomer_undervoltage_recovery_voltage.name = "monomer_undervoltage_recovery_voltage";
    monomer_undervoltage_recovery_voltage.code = "0x94";
    monomer_undervoltage_recovery_voltage.description = "Monomer Undervoltage Recovery Voltage";
    monomer_undervoltage_recovery_voltage.index = 75;
    monomer_undervoltage_recovery_voltage.data_length = 2;
    monomer_undervoltage_recovery_voltage.label = "Monomer Undervoltage Recovery Voltage";
    monomer_undervoltage_recovery_voltage.unit = "mV";
    monomer_undervoltage_recovery_voltage.type = "int";
    monomer_undervoltage_recovery_voltage.scale = 1.0;
    monomer_undervoltage_recovery_voltage.signed_value = false;
    config.dataFields["monomer_undervoltage_recovery_voltage"] = monomer_undervoltage_recovery_voltage;

    DataFieldConfig single_undervoltage_protection_delay;
    single_undervoltage_protection_delay.name = "single_undervoltage_protection_delay";
    single_undervoltage_protection_delay.code = "0x95";
    single_undervoltage_protection_delay.description = "Single Undervoltage Protection Delay";
    single_undervoltage_protection_delay.index = 77;
    single_undervoltage_protection_delay.data_length = 2;
    single_undervoltage_protection_delay.label = "Single Undervoltage Protection Delay";
    single_undervoltage_protection_delay.unit = "seconds";
    single_undervoltage_protection_delay.type = "int";
    single_undervoltage_protection_delay.scale = 1.0;
    single_undervoltage_protection_delay.signed_value = false;
    config.dataFields["single_undervoltage_protection_delay"] = single_undervoltage_protection_delay;

    DataFieldConfig cell_pressure_difference_protection;
    cell_pressure_difference_protection.name = "cell_pressure_difference_protection";
    cell_pressure_difference_protection.code = "0x96";
    cell_pressure_difference_protection.description = "Cell Pressure Difference Protection";
    cell_pressure_difference_protection.index = 79;
    cell_pressure_difference_protection.data_length = 2;
    cell_pressure_difference_protection.label = "Cell Pressure Difference Protection";
    cell_pressure_difference_protection.unit = "mV";
    cell_pressure_difference_protection.type = "int";
    cell_pressure_difference_protection.scale = 1.0;
    cell_pressure_difference_protection.signed_value = false;
    config.dataFields["cell_pressure_difference_protection"] = cell_pressure_difference_protection;

    DataFieldConfig discharge_overcurrent_protection_value;
    discharge_overcurrent_protection_value.name = "discharge_overcurrent_protection_value";
    discharge_overcurrent_protection_value.code = "0x97";
    discharge_overcurrent_protection_value.description = "Discharge Overcurrent Protection Value";
    discharge_overcurrent_protection_value.index = 81;
    discharge_overcurrent_protection_value.data_length = 2;
    discharge_overcurrent_protection_value.label = "Discharge Overcurrent Protection Value";
    discharge_overcurrent_protection_value.unit = "A";
    discharge_overcurrent_protection_value.type = "float";
    discharge_overcurrent_protection_value.scale = 1.0;
    discharge_overcurrent_protection_value.signed_value = false;
    config.dataFields["discharge_overcurrent_protection_value"] = discharge_overcurrent_protection_value;

    DataFieldConfig discharge_overcurrent_delay;
    discharge_overcurrent_delay.name = "discharge_overcurrent_delay";
    discharge_overcurrent_delay.code = "0x98";
    discharge_overcurrent_delay.description = "Discharge Overcurrent Delay";
    discharge_overcurrent_delay.index = 83;
    discharge_overcurrent_delay.data_length = 2;
    discharge_overcurrent_delay.label = "Discharge Overcurrent Delay";
    discharge_overcurrent_delay.unit = "seconds";
    discharge_overcurrent_delay.type = "int";
    discharge_overcurrent_delay.scale = 1.0;
    discharge_overcurrent_delay.signed_value = false;
    config.dataFields["discharge_overcurrent_delay"] = discharge_overcurrent_delay;

    DataFieldConfig charging_overcurrent_protection_value;
    charging_overcurrent_protection_value.name = "charging_overcurrent_protection_value";
    charging_overcurrent_protection_value.code = "0x99";
    charging_overcurrent_protection_value.description = "Charging Overcurrent Protection Value";
    charging_overcurrent_protection_value.index = 85;
    charging_overcurrent_protection_value.data_length = 2;
    charging_overcurrent_protection_value.label = "Charging Overcurrent Protection Value";
    charging_overcurrent_protection_value.unit = "A";
    charging_overcurrent_protection_value.type = "float";
    charging_overcurrent_protection_value.scale = 1.0;
    charging_overcurrent_protection_value.signed_value = false;
    config.dataFields["charging_overcurrent_protection_value"] = charging_overcurrent_protection_value;

    DataFieldConfig charge_overcurrent_delay;
    charge_overcurrent_delay.name = "charge_overcurrent_delay";
    charge_overcurrent_delay.code = "0x9A";
    charge_overcurrent_delay.description = "Charge Overcurrent Delay";
    charge_overcurrent_delay.index = 87;
    charge_overcurrent_delay.data_length = 2;
    charge_overcurrent_delay.label = "Charge Overcurrent Delay";
    charge_overcurrent_delay.unit = "seconds";
    charge_overcurrent_delay.type = "int";
    charge_overcurrent_delay.scale = 1.0;
    charge_overcurrent_delay.signed_value = false;
    config.dataFields["charge_overcurrent_delay"] = charge_overcurrent_delay;

    DataFieldConfig balanced_starting_voltage;
    balanced_starting_voltage.name = "balanced_starting_voltage";
    balanced_starting_voltage.code = "0x9B";
    balanced_starting_voltage.description = "Balanced Starting Voltage";
    balanced_starting_voltage.index = 89;
    balanced_starting_voltage.data_length = 2;
    balanced_starting_voltage.label = "Balanced Starting Voltage";
    balanced_starting_voltage.unit = "mV";
    balanced_starting_voltage.type = "int";
    balanced_starting_voltage.scale = 1.0;
    balanced_starting_voltage.signed_value = false;
    config.dataFields["balanced_starting_voltage"] = balanced_starting_voltage;

    DataFieldConfig balanced_opening_pressure_difference;
    balanced_opening_pressure_difference.name = "balanced_opening_pressure_difference";
    balanced_opening_pressure_difference.code = "0x9C";
    balanced_opening_pressure_difference.description = "Balanced Opening Pressure Difference";
    balanced_opening_pressure_difference.index = 91;
    balanced_opening_pressure_difference.data_length = 2;
    balanced_opening_pressure_difference.label = "Balanced Opening Pressure Difference";
    balanced_opening_pressure_difference.unit = "mV";
    balanced_opening_pressure_difference.type = "int";
    balanced_opening_pressure_difference.scale = 1.0;
    balanced_opening_pressure_difference.signed_value = false;
    config.dataFields["balanced_opening_pressure_difference"] = balanced_opening_pressure_difference;

    DataFieldConfig active_balance_switch;
    active_balance_switch.name = "active_balance_switch";
    active_balance_switch.code = "0x9D";
    active_balance_switch.description = "Active Balance Switch";
    active_balance_switch.index = 93;
    active_balance_switch.data_length = 1;
    active_balance_switch.label = "Active Balance Switch";
    active_balance_switch.type = "int";
    active_balance_switch.scale = 1.0;
    active_balance_switch.signed_value = false;
    config.dataFields["active_balance_switch"] = active_balance_switch;

    DataFieldConfig power_tube_temperature_protection_value;
    power_tube_temperature_protection_value.name = "power_tube_temperature_protection_value";
    power_tube_temperature_protection_value.code = "0x9E";
    power_tube_temperature_protection_value.description = "Power Tube Temperature Protection Value";
    power_tube_temperature_protection_value.index = 94;
    power_tube_temperature_protection_value.data_length = 2;
    power_tube_temperature_protection_value.label = "Power Tube Temperature Protection Value";
    power_tube_temperature_protection_value.unit = "°C";
    power_tube_temperature_protection_value.type = "int";
    power_tube_temperature_protection_value.scale = 1.0;
    power_tube_temperature_protection_value.signed_value = false;
    config.dataFields["power_tube_temperature_protection_value"] = power_tube_temperature_protection_value;

    DataFieldConfig power_tube_temperature_recovery_value;
    power_tube_temperature_recovery_value.name = "power_tube_temperature_recovery_value";
    power_tube_temperature_recovery_value.code = "0x9F";
    power_tube_temperature_recovery_value.description = "Power Tube Temperature Recovery Value";
    power_tube_temperature_recovery_value.index = 96;
    power_tube_temperature_recovery_value.data_length = 2;
    power_tube_temperature_recovery_value.label = "Power Tube Temperature Recovery Value";
    power_tube_temperature_recovery_value.unit = "°C";
    power_tube_temperature_recovery_value.type = "int";
    power_tube_temperature_recovery_value.scale = 1.0;
    power_tube_temperature_recovery_value.signed_value = false;
    config.dataFields["power_tube_temperature_recovery_value"] = power_tube_temperature_recovery_value;

    DataFieldConfig temperature_protection_value_in_the_battery_box;
    temperature_protection_value_in_the_battery_box.name = "temperature_protection_value_in_the_battery_box";
    temperature_protection_value_in_the_battery_box.code = "0xA0";
    temperature_protection_value_in_the_battery_box.description = "Temperature Protection Value in the Battery Box";
    temperature_protection_value_in_the_battery_box.index = 98;
    temperature_protection_value_in_the_battery_box.data_length = 2;
    temperature_protection_value_in_the_battery_box.label = "Temperature Protection Value in the Battery Box";
    temperature_protection_value_in_the_battery_box.unit = "°C";
    temperature_protection_value_in_the_battery_box.type = "int";
    temperature_protection_value_in_the_battery_box.scale = 1.0;
    temperature_protection_value_in_the_battery_box.signed_value = false;
    config.dataFields["temperature_protection_value_in_the_battery_box"] = temperature_protection_value_in_the_battery_box;

    DataFieldConfig temperature_recovery_value_in_the_battery_box;
    temperature_recovery_value_in_the_battery_box.name = "temperature_recovery_value_in_the_battery_box";
    temperature_recovery_value_in_the_battery_box.code = "0xA1";
    temperature_recovery_value_in_the_battery_box.description = "Temperature Recovery Value in the Battery Box";
    temperature_recovery_value_in_the_battery_box.index = 100;
    temperature_recovery_value_in_the_battery_box.data_length = 2;
    temperature_recovery_value_in_the_battery_box.label = "Temperature Recovery Value in the Battery Box";
    temperature_recovery_value_in_the_battery_box.unit = "°C";
    temperature_recovery_value_in_the_battery_box.type = "int";
    temperature_recovery_value_in_the_battery_box.scale = 1.0;
    temperature_recovery_value_in_the_battery_box.signed_value = false;
    config.dataFields["temperature_recovery_value_in_the_battery_box"] = temperature_recovery_value_in_the_battery_box;

    DataFieldConfig battery_temperature_difference_protection_value;
    battery_temperature_difference_protection_value.name = "battery_temperature_difference_protection_value";
    battery_temperature_difference_protection_value.code = "0xA2";
    battery_temperature_difference_protection_value.description = "Battery Temperature Difference Protection Value";
    battery_temperature_difference_protection_value.index = 102;
    battery_temperature_difference_protection_value.data_length = 2;
    battery_temperature_difference_protection_value.label = "Battery Temperature Difference Protection Value";
    battery_temperature_difference_protection_value.unit = "°C";
    battery_temperature_difference_protection_value.type = "int";
    battery_temperature_difference_protection_value.scale = 1.0;
    battery_temperature_difference_protection_value.signed_value = false;
    config.dataFields["battery_temperature_difference_protection_value"] = battery_temperature_difference_protection_value;

    DataFieldConfig battery_charging_high_temperature_protection_value;
    battery_charging_high_temperature_protection_value.name = "battery_charging_high_temperature_protection_value";
    battery_charging_high_temperature_protection_value.code = "0xA3";
    battery_charging_high_temperature_protection_value.description = "Battery Charging High Temperature Protection Value";
    battery_charging_high_temperature_protection_value.index = 104;
    battery_charging_high_temperature_protection_value.data_length = 2;
    battery_charging_high_temperature_protection_value.label = "Battery Charging High Temperature Protection Value";
    battery_charging_high_temperature_protection_value.unit = "°C";
    battery_charging_high_temperature_protection_value.type = "int";
    battery_charging_high_temperature_protection_value.scale = 1.0;
    battery_charging_high_temperature_protection_value.signed_value = false;
    config.dataFields["battery_charging_high_temperature_protection_value"] = battery_charging_high_temperature_protection_value;

    DataFieldConfig battery_discharge_high_temperature_protection_value;
    battery_discharge_high_temperature_protection_value.name = "battery_discharge_high_temperature_protection_value";
    battery_discharge_high_temperature_protection_value.code = "0xA4";
    battery_discharge_high_temperature_protection_value.description = "Battery Discharge High Temperature Protection Value";
    battery_discharge_high_temperature_protection_value.index = 106;
    battery_discharge_high_temperature_protection_value.data_length = 2;
    battery_discharge_high_temperature_protection_value.label = "Battery Discharge High Temperature Protection Value";
    battery_discharge_high_temperature_protection_value.unit = "°C";
    battery_discharge_high_temperature_protection_value.type = "int";
    battery_discharge_high_temperature_protection_value.scale = 1.0;
    battery_discharge_high_temperature_protection_value.signed_value = false;
    config.dataFields["battery_discharge_high_temperature_protection_value"] = battery_discharge_high_temperature_protection_value;

    DataFieldConfig charging_low_temperature_protection_value;
    charging_low_temperature_protection_value.name = "charging_low_temperature_protection_value";
    charging_low_temperature_protection_value.code = "0xA5";
    charging_low_temperature_protection_value.description = "Charging Low Temperature Protection Value";
    charging_low_temperature_protection_value.index = 108;
    charging_low_temperature_protection_value.data_length = 2;
    charging_low_temperature_protection_value.label = "Charging Low Temperature Protection Value";
    charging_low_temperature_protection_value.unit = "°C";
    charging_low_temperature_protection_value.type = "int";
    charging_low_temperature_protection_value.scale = 1.0;
    charging_low_temperature_protection_value.signed_value = false;
    config.dataFields["charging_low_temperature_protection_value"] = charging_low_temperature_protection_value;

    DataFieldConfig charging_low_temperature_protection_recovery_value;
    charging_low_temperature_protection_recovery_value.name = "charging_low_temperature_protection_recovery_value";
    charging_low_temperature_protection_recovery_value.code = "0xA6";
    charging_low_temperature_protection_recovery_value.description = "Charging Low Temperature Protection Recovery Value";
    charging_low_temperature_protection_recovery_value.index = 110;
    charging_low_temperature_protection_recovery_value.data_length = 2;
    charging_low_temperature_protection_recovery_value.label = "Charging Low Temperature Protection Recovery Value";
    charging_low_temperature_protection_recovery_value.unit = "°C";
    charging_low_temperature_protection_recovery_value.type = "int";
    charging_low_temperature_protection_recovery_value.scale = 1.0;
    charging_low_temperature_protection_recovery_value.signed_value = false;
    config.dataFields["charging_low_temperature_protection_recovery_value"] = charging_low_temperature_protection_recovery_value;

    DataFieldConfig discharge_low_temperature_protection_value;
    discharge_low_temperature_protection_value.name = "discharge_low_temperature_protection_value";
    discharge_low_temperature_protection_value.code = "0xA7";
    discharge_low_temperature_protection_value.description = "Discharge Low Temperature Protection Value";
    discharge_low_temperature_protection_value.index = 112;
    discharge_low_temperature_protection_value.data_length = 2;
    discharge_low_temperature_protection_value.label = "Discharge Low Temperature Protection Value";
    discharge_low_temperature_protection_value.unit = "°C";
    discharge_low_temperature_protection_value.type = "int";
    discharge_low_temperature_protection_value.scale = 1.0;
    discharge_low_temperature_protection_value.signed_value = false;
    config.dataFields["discharge_low_temperature_protection_value"] = discharge_low_temperature_protection_value;

    DataFieldConfig discharge_low_temperature_protection_recovery_value;
    discharge_low_temperature_protection_recovery_value.name = "discharge_low_temperature_protection_recovery_value";
    discharge_low_temperature_protection_recovery_value.code = "0xA8";
    discharge_low_temperature_protection_recovery_value.description = "Discharge Low Temperature Protection Recovery Value";
    discharge_low_temperature_protection_recovery_value.index = 114;
    discharge_low_temperature_protection_recovery_value.data_length = 2;
    discharge_low_temperature_protection_recovery_value.label = "Discharge Low Temperature Protection Recovery Value";
    discharge_low_temperature_protection_recovery_value.unit = "°C";
    discharge_low_temperature_protection_recovery_value.type = "int";
    discharge_low_temperature_protection_recovery_value.scale = 1.0;
    discharge_low_temperature_protection_recovery_value.signed_value = false;
    config.dataFields["discharge_low_temperature_protection_recovery_value"] = discharge_low_temperature_protection_recovery_value;

    DataFieldConfig battery_string_setting;
    battery_string_setting.name = "battery_string_setting";
    battery_string_setting.code = "0xA9";
    battery_string_setting.description = "Battery String Setting";
    battery_string_setting.index = 116;
    battery_string_setting.data_length = 1;
    battery_string_setting.label = "Battery String Setting";
    battery_string_setting.type = "int";
    battery_string_setting.scale = 1.0;
    battery_string_setting.signed_value = false;
    config.dataFields["battery_string_setting"] = battery_string_setting;

    DataFieldConfig battery_capacity_setting;
    battery_capacity_setting.name = "battery_capacity_setting";
    battery_capacity_setting.code = "0xAA";
    battery_capacity_setting.description = "Battery Capacity Setting";
    battery_capacity_setting.index = 117;
    battery_capacity_setting.data_length = 4;
    battery_capacity_setting.label = "Battery Capacity Setting";
    battery_capacity_setting.unit = "Ah";
    battery_capacity_setting.type = "float";
    battery_capacity_setting.scale = 1.0;
    battery_capacity_setting.signed_value = false;
    config.dataFields["battery_capacity_setting"] = battery_capacity_setting;

    DataFieldConfig charging_mos_tube_switch;
    charging_mos_tube_switch.name = "charging_mos_tube_switch";
    charging_mos_tube_switch.code = "0xAB";
    charging_mos_tube_switch.description = "Charging MOS Tube Switch";
    charging_mos_tube_switch.index = 121;
    charging_mos_tube_switch.data_length = 1;
    charging_mos_tube_switch.label = "Charging MOS Tube Switch";
    charging_mos_tube_switch.type = "int";
    charging_mos_tube_switch.scale = 1.0;
    charging_mos_tube_switch.signed_value = false;
    config.dataFields["charging_mos_tube_switch"] = charging_mos_tube_switch;

    DataFieldConfig discharge_mos_tube_switch;
    discharge_mos_tube_switch.name = "discharge_mos_tube_switch";
    discharge_mos_tube_switch.code = "0xAC";
    discharge_mos_tube_switch.description = "Discharge MOS Tube Switch";
    discharge_mos_tube_switch.index = 122;
    discharge_mos_tube_switch.data_length = 1;
    discharge_mos_tube_switch.label = "Discharge MOS Tube Switch";
    discharge_mos_tube_switch.type = "int";
    discharge_mos_tube_switch.scale = 1.0;
    discharge_mos_tube_switch.signed_value = false;
    config.dataFields["discharge_mos_tube_switch"] = discharge_mos_tube_switch;

    DataFieldConfig current_calibration;
    current_calibration.name = "current_calibration";
    current_calibration.code = "0xAD";
    current_calibration.description = "Current Calibration";
    current_calibration.index = 123;
    current_calibration.data_length = 2;
    current_calibration.label = "Current Calibration";
    current_calibration.unit = "mA";
    current_calibration.type = "int";
    current_calibration.scale = 1.0;
    current_calibration.signed_value = false;
    config.dataFields["current_calibration"] = current_calibration;

    DataFieldConfig protection_board_address;
    protection_board_address.name = "protection_board_address";
    protection_board_address.code = "0xAE";
    protection_board_address.description = "Protection Board Address";
    protection_board_address.index = 125;
    protection_board_address.data_length = 1;
    protection_board_address.label = "Protection Board Address";
    protection_board_address.type = "int";
    protection_board_address.scale = 1.0;
    protection_board_address.signed_value = false;
    config.dataFields["protection_board_address"] = protection_board_address;

    DataFieldConfig battery_type;
    battery_type.name = "battery_type";
    battery_type.code = "0xAF";
    battery_type.description = "Battery Type";
    battery_type.index = 126;
    battery_type.data_length = 1;
    battery_type.label = "Battery Type";
    battery_type.type = "int";
    battery_type.scale = 1.0;
    battery_type.signed_value = false;
    config.dataFields["battery_type"] = battery_type;

    DataFieldConfig sleep_waiting_time;
    sleep_waiting_time.name = "sleep_waiting_time";
    sleep_waiting_time.code = "0xB0";
    sleep_waiting_time.description = "Sleep Waiting Time";
    sleep_waiting_time.index = 127;
    sleep_waiting_time.data_length = 2;
    sleep_waiting_time.label = "Sleep Waiting Time";
    sleep_waiting_time.unit = "seconds";
    sleep_waiting_time.type = "int";
    sleep_waiting_time.scale = 1.0;
    sleep_waiting_time.signed_value = false;
    config.dataFields["sleep_waiting_time"] = sleep_waiting_time;

    DataFieldConfig low_volume_alarm_value;
    low_volume_alarm_value.name = "low_volume_alarm_value";
    low_volume_alarm_value.code = "0xB1";
    low_volume_alarm_value.description = "Low Volume Alarm Value";
    low_volume_alarm_value.index = 129;
    low_volume_alarm_value.data_length = 1;
    low_volume_alarm_value.label = "Low Volume Alarm Value";
    low_volume_alarm_value.unit = "%";
    low_volume_alarm_value.type = "int";
    low_volume_alarm_value.scale = 1.0;
    low_volume_alarm_value.signed_value = false;
    config.dataFields["low_volume_alarm_value"] = low_volume_alarm_value;

    DataFieldConfig system_working_hours;
    system_working_hours.name = "system_working_hours";
    system_working_hours.code = "0xB6";
    system_working_hours.description = "System Working Hours";
    system_working_hours.index = 130;
    system_working_hours.data_length = 4;
    system_working_hours.label = "System Working Hours";
    system_working_hours.unit = "Minutes";
    system_working_hours.type = "long";
    system_working_hours.scale = 1.0;
    system_working_hours.signed_value = false;
    config.dataFields["system_working_hours"] = system_working_hours;

    DataFieldConfig actual_battery_capacity;
    actual_battery_capacity.name = "actual_battery_capacity";
    actual_battery_capacity.code = "0xB9";
    actual_battery_capacity.description = "Actual Battery Capacity";
    actual_battery_capacity.index = 134;
    actual_battery_capacity.data_length = 4;
    actual_battery_capacity.label = "Actual Battery Capacity";
    actual_battery_capacity.unit = "Ah";
    actual_battery_capacity.type = "float";
    actual_battery_capacity.scale = 1.0;
    actual_battery_capacity.signed_value = false;
    config.dataFields["actual_battery_capacity"] = actual_battery_capacity;

    DataFieldConfig manufacturer_id;
    manufacturer_id.name = "manufacturer_id";
    manufacturer_id.code = "0xBA";
    manufacturer_id.description = "Manufacturer ID";
    manufacturer_id.index = 138;
    manufacturer_id.data_length = 24;
    manufacturer_id.label = "Manufacturer ID";
    manufacturer_id.type = "string";
    manufacturer_id.scale = 1.0;
    manufacturer_id.signed_value = false;
    config.dataFields["manufacturer_id"] = manufacturer_id;

    return config;
}

#endif // STRUCTS_H
