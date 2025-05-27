#!/bin/bash

# Define the base URL
IP="192.168.1.3"
BASE_URL="http://$IP/"

# Flyback Get Endpoints
FLYBACK_GETS=("get_flyback_voltage" "get_flyback_current" "get_flyback_power" "get_flyback_frequency")

# Flyback Set Endpoints with realistic values (using slash format)
FLYBACK_SETS=("set_flyback_frequency/50" "set_flyback_dutyCycle/30")

# VAT Get Endpoints
VAT_GETS=("get_actual_position" "get_actual_pressure")

# VAT Set Endpoints with realistic values (using slash format)
VAT_SETS=("set_control_mode/1" "set_target_pressure/5")

# Scenario Set Endpoints
SCENARIO_SETS=("set_scenario/1" "set_scenario/2" "set_scenario/3" "set_scenario/4" "set_scenario/5")

# Sensor Endpoints
SENSOR_GETS=("get_temperature_MCP9601C_Indoor" "get_temperature_MCP9601F_Indoor" "get_temperature_MCP9601K_Indoor" "get_temperature_MCP9601C_Outdoor" "get_temperature_MCP9601F_Outdoor" get_temperature_MCP9601K_Outdoor)

# Generic Get/Set Endpoints with realistic values (using slash format)
GENERIC_GETS=("get_actual_position" "get_actual_pressure")
GENERIC_SETS=("set_control_mode/1" "set_target_pressure/10")

# Reportsystem Endpoints
REPORTSYSTEM_GETS=("get_report_stackOverflow" "get_report_ethernet" "get_report_spi" "get_report_i2c" "get_report_temp" "get_report_press")

# Function to execute curl command for each endpoint with delay
execute_curl() {
    for endpoint in "$@"; do
        echo "Sending request to: $BASE_URL$endpoint"
        curl "$BASE_URL$endpoint"
        echo
        sleep 0.200  # Adds a 500 ms delay (adjust as needed)
    done
}

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        --flyback-get)
            echo "Executing Flyback Get Endpoints..."
            execute_curl "${FLYBACK_GETS[@]}"
            shift
            ;;
        --flyback-set)
            echo "Executing Flyback Set Endpoints..."
            execute_curl "${FLYBACK_SETS[@]}"
            shift
            ;;
        --vat-get)
            echo "Executing VacControl Get Endpoints..."
            execute_curl "${VAT_GETS[@]}"
            shift
            ;;
        --vat-set)
            echo "Executing VacControl Set Endpoints..."
            execute_curl "${VAT_SETS[@]}"
            shift
            ;;
        --scenario-set)
            echo "Executing Scenario Set Endpoints..."
            execute_curl "${SCENARIO_SETS[@]}"
            shift
            ;;
        --sensor-get)
            echo "Executing Sensor Get Endpoints..."
            execute_curl "${SENSOR_GETS[@]}"
            shift
            ;;
        --generic-get)
            echo "Executing Generic Get Endpoints..."
            execute_curl "${GENERIC_GETS[@]}"
            shift
            ;;
        --generic-set)
            echo "Executing Generic Set Endpoints..."
            execute_curl "${GENERIC_SETS[@]}"
            shift
            ;;
        --report-get)
            echo "Executing Report System Get Endpoints..."
            execute_curl "${REPORTSYSTEM_GETS[@]}"
            shift
            ;;
        --all-getset)
            echo "Executing All Get/Set Endpoints..."
            execute_curl "${FLYBACK_GETS[@]}" "${FLYBACK_SETS[@]}" "${VAT_GETS[@]}" "${VAT_SETS[@]}" "${SCENARIO_SETS[@]}" "${SENSOR_GETS[@]}" "${GENERIC_GETS[@]}" "${GENERIC_SETS[@]} ${REPORTSYSTEM_GETS[@]}"
            shift
            ;;
        *)
            echo "Invalid option: $1"
            exit 1
            ;;
    esac
done

echo "Done."
