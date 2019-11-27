//  ***************************************************************************
/// @file    veeprom_map.h
/// @author  NeoProg
/// @brief   VEEPROM mapping
//  ***************************************************************************
#ifndef VEEPROM_MAP_H_
#define VEEPROM_MAP_H_

// Base addresses for limbs configuration
#define LIMB_COXA_LENGTH_EE_ADDRESS                     (0x0000)
#define LIMB_FEMUR_LENGTH_EE_ADDRESS                    (0x0002)
#define LIMB_TIBIA_LENGTH_EE_ADDRESS                    (0x0004)
#define LIMB_COXA_ZERO_ROTATE_EE_ADDRESS                (0x0006)
#define LIMB_FEMUR_ZERO_ROTATE_EE_ADDRESS               (0x0008)
#define LIMB_TIBIA_ZERO_ROTATE_EE_ADDRESS               (0x000A)
#define LIMB_COXA_MIN_ANGLE_EE_ADDRESS                  (0x0010)
#define LIMB_COXA_MAX_ANGLE_EE_ADDRESS                  (0x0011)
#define LIMB_FEMUR_MIN_ANGLE_EE_ADDRESS                 (0x0012)
#define LIMB_FEMUR_MAX_ANGLE_EE_ADDRESS                 (0x0013)
#define LIMB_TIBIA_MIN_ANGLE_EE_ADDRESS                 (0x0014)
#define LIMB_TIBIA_MAX_ANGLE_EE_ADDRESS                 (0x0015)
#define LIMB_START_POSITION_X_EE_ADDRESS                (0x0016)
#define LIMB_START_POSITION_Y_EE_ADDRESS                (0x0018)
#define LIMB_START_POSITION_Z_EE_ADDRESS                (0x001A)
#define LIMB_CONFIGURATION_SIZE                         (32)


// Base addresses for servos configuration
#define SERVO_CONFIGURATION_BASE_EE_ADDRESS             (0x0100)
#define SERVO_CONFIG_OFFSET                             (0)        ///< U8  Servo algorithm type
#define     SERVO_CONFIG_ROTATE_DIRECTION_MASK          (0x01)
#define     SERVO_CONFIG_BIDIRECTIONAL_MODE_MASK        (0x02)
#define SERVO_ANGLE_CORRECTION_OFFSET                   (1)        ///< U8  Servo rotate direction
#define SERVO_MAX_PHYSIC_ANGLE_OFFSET                   (4)        ///< U16 Start max physic angle
#define SERVO_CALIBRATION_TABLE_OFFSET                  (8)        ///< U16 First calibration table value
#define SERVO_CONFIGURATION_SIZE                        (64)


#define VOLTAGE_DIVISOR_CONFIGURATION_BASE_EE_ADDRESS   (0x0600)
#define VOLTAGE_DIVISOR_UP_RESIST_OFFSET                (0)
#define VOLTAGE_DIVISOR_DOWN_RESIST_OFFSET              (4)
#define VOLTAGE_DIVISOR_CONFIGURATION_SIZE              (8)

#define BATTERY_LOW_VOLTAGE_THRESHOLD_EE_ADDRESS        (0x0650)
#define FRONT_DISTANCE_LOW_LIMIT_EE_ADDRESS             (0x0660)


#endif /* VEEPROM_MAP_H_ */