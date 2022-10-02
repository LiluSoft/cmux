// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
/// \endcond

/**
 * @file parameter_negotiation.h
 * @brief parameter negotiation codec
 * 
 * @details The request primitive is used to request that the remote 07.10 entity changes a specific DLC connection
 * parameters. An indication is sent to the remote port emulation entity. The remote emulation entity replies with a
 * response which is forwarded as an confirmation to the originating port emulating entity.
 * 
 * See 5.1.8.1.1 DLC parameter negotiation
 */

/**
 * @brief Types of frames used for carrying information
 *
 */
enum parameter_frame_types
{
    FRAME_TYPE_UIH = 0b00000000,
    FRAME_TYPE_UI = 0b00000001,
    FRAME_TYPE_I = 0b00000010
};

/**
 * @brief Convergence Layers
 * 
 * @details Convergence layers are defined to permit data which has implied structure to be conveyed through the multiplexer
 * without losing the structure or other parameters which are associated with the data stream. Common uses of
 * convergence layers are to carry the state of V.24 control signals through a DLC or to ensure that the boundaries of a
 * coded voice frame are preserved.
 * 
 * Convergence layers apply to data whether it is carried by error recovery mode or non-error recovery mode procedures.
 * The use of particular convergence layers is implicitly linked to the DLCIs used but may be negotiated away from these
 * defaults by the use of the multiplexer control channel. 
 * 
 * See 5.5 Convergence Layers
 * 
 */
enum convergence_layer_type
{
    /**
     * @brief Type 1 - Unstructured Octet Stream
     * 
     * @details Data which consists of an unstructured sequence of octets, for example, 64 kbit/s uncoded voice or normal
     * asynchronous data without V.24 control signals, is inserted directly into the I-field. In this case, it could be said that the
     * convergence layer is null.
     * 
     * Type 1 is the default convergence layer for each DLC. 
     * 
     * See 5.5.1 Type 1 - Unstructured Octet Stream
     */
    LAYER_Type1 = 0b00000000,

    /**
     * @brief Type 2 - Unstructured Octet Stream with flow control, break signal handling and transmission of V.24 signal states
     * 
     * @details If it is desired to convey virtual V.24 control signals associated with a data stream the first octet of each I-field contains
     * a representation of the state of the signals in accordance with figure 15. The use of the extension bit allows other octets
     * to be added to cater for other circumstances. At present, an optional second octet is defined for handling the
     * transmission of break signals. 
     * 
     * See 5.5.2 Type 2 - Unstructured Octet Stream with flow control, break signal handling and transmission of V.24 signal states
     */
    LAYER_Type2 = 0b00000001,

    /**
     * @brief Type 3 - Uninterruptible Framed Data
     * 
     * @details An example of uninterruptible framed data is coded voice data made up of a sequence of voice frames. It is important
     * that coded voice frames reach the voice decoder with the frame structure intact and with the shortest possible delay. The
     * simplest way of ensuring this is to map one complete voice frame into one I-field. This frame shall not be shortened
     * during transmission even if data of higher priority is waiting.
     * 
     * At the transmitter each frame of data is inserted into the I field of an I frame, UI frame or UIH frame. The data shall not
     * be spread over more than one frame and data from other frames must not be included in the I field. The receiver handles
     * the data as a complete frame and passes it on as a complete frame.
     * Coded voice data should be transmitted using UI frames or UIH frames because the delays associated with
     * re-transmissions are usually unacceptable. 
     * 
     * See 5.5.3 Type 3 - Uninterruptible Framed Data
     */
    LAYER_Type3 = 0b00000010,

    /**
     * @brief Type 4 - Interruptable Framed Data
     * 
     * @details This type of convergence layer is used might be used to convey data which has an implied structure but where the delay
     * is not as important as Type 3. The structured data may be segmented across several frames and re-assembled at the
     * other station. PPP-framed IP data is an example of the type of data that could be carried over a Type 4 convergence
     * layer.
     * 
     * See 5.5.4 Type 4 - Interruptable Framed Data
     */
    LAYER_Type4 = 0b00000011
};

/**
 * @brief parameter negotiation parameters
 * 
 */
struct parameter_negotiation_parameters_t
{
    /**
     * @brief DLCI
     *
     */
    uint8_t dlci;

    /**
     * @brief e type of frames used for carrying information in the particular DLC
     *
     * Other values are reserved, default value is UIH
     */
    enum parameter_frame_types frame_type;

    /**
     * @brief type of convergence layer to be used on the particular DLCI
     *
     * Other values are reserved.
     * Default value is Type1
     */
    enum convergence_layer_type layer_type;

    /**
     * @brief priority to be assigned to the particular DLC.
     *
     * The range of values is 0 to 63 with 0 being the lowest priority.
     *
     * See clause 5.6.
     *
     */
    uint8_t priority;

    /**
     * @brief acknowledgement timer (T1) - see clause 5.7.1.
     * The resolution is hundredths of a second
     *
     * 10 ms - 25.5 sec,
     * default: 100 ms
     */
    uint32_t ack_timer_msec;

    /**
     * @brief maximum frame size (N1)
     *
     * Values 1 – 32768, default: 31 for the basic option and 64 for the advanced option
     *
     * see clause 5.7.2.
     *
     */
    uint16_t max_frame_size;

    /**
     * @brief maximum number of retransmissions (N2)
     *
     * Values 0 – 100, default : 3
     *
     * see clause 5.7.3.
     *
     */
    uint8_t max_retransmissions;

    /**
     * @brief window size for error recovery mode (k)
     *
     * Values 1 – 7, default: 2
     * see clause 5.7.4.
     *
     */
    uint8_t window_size;
};

/**
 * @brief decodes octets into parameter negotiation
 * 
 * @param octets 
 * @param parameters parameter negotiation
 */
void parameter_negotiation_decode(const uint8_t octets[8], struct parameter_negotiation_parameters_t *parameters);

/**
 * @brief dumps parameter negotiation
 * 
 * @param DLC_parameters parameter negotiation
 */
void parameter_negotiation_dump(struct parameter_negotiation_parameters_t DLC_parameters);

/**
 * @brief get parameter negotiation encode length
 * 
 * @param parameters parameter negotiation
 * @return size_t number of bytes
 */
size_t parameter_negotiation_encode_length(struct parameter_negotiation_parameters_t *parameters);

/**
 * @brief encode parameter negotiation into octets
 * 
 * @param octets 
 * @param parameters parameter negotiation
 */
void parameter_negotiation_encode(uint8_t *octets, struct parameter_negotiation_parameters_t *parameters);
