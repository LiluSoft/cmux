// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/**
 * @file cmux_frame_type.h
 * @brief CMUX Frame Type
 *
 */

/**
 * @brief Frame Types
 *
 */
enum CONTROL_TYPE
{
    /**
     * @brief SABM (Set Asynchronous Balanced Mode)
     * 
     * The SABM command shall be used to place the addressed station in the Asynchronous Balanced Mode (ABM) where
     * all control fields shall be one octet in length. The station shall confirm acceptance of the SABM command by
     * transmission of a UA response at the first opportunity. Upon acceptance of this command, the DLC send and receive
     * state variables shall be set to zero.
     *
     */
    SABM = 0b00101111,
    /**
     * @brief UA (Unnumbered Acknowledgement)
     * 
     * The UA response shall be used by the station to acknowledge the receipt and acceptance of SABM and DISC
     * commands.
     *
     */
    UA = 0b01100011,
    /**
     * @brief DM (Disconnected Mode)
     * 
     * The DM response shall be used to report a status where the station is logically disconnected from the data link. When in
     * disconnected mode no commands are accepted until the disconnected mode is terminated by the receipt of a SABM
     * command. If a DISC command is received while in disconnected mode a DM response should be sent.
     *
     */
    DM = 0b00001111,
    /**
     * @brief DISC (Disconnect)
     * 
     * The DISC command shall be used to terminate an operational or initialization mode previously set by a command. It
     * shall be used to inform one station that the other station is suspending operation and that the station should assume a
     * logically disconnected mode. Prior to actioning the command, the receiving station shall confirm the acceptance of the
     * DISC command by the transmission of a UA response.
     * DISC command sent at DLCI 0 have the same meaning as the Multiplexer Close Down command (see subclause
     * 5.4.6.3.3). See also subclause 5.8.2 for more information about the Close-down procedure.
     *
     */
    DISC = 0b01000011,
    /**
     * @brief UIH (Unnumbered Information with Header check)
     *
     * The UIH command/response shall be used to send information without affecting the V(S) or V(R) variables at either
     * station. UIH is used where the integrity of the information being transferred is of lesser importance than its delivery to
     * the correct DLCI. For the UIH frame, the FCS shall be calculated over only the address and control fields.
     * Reception of the UIH command/response is not sequence number verified by the data link procedures; therefore, the
     * UIH frame may be lost if a data link exception occurs during transmission of the protected portion of the command, or
     * duplicated if an exception condition occurs during any reply to the command. There is no specified response to the UIH
     * command/response.
     */
    UIH = 0b11101111,
    /**
     * @brief UI (Unnumbered Information)
     *
     * The UI command/response shall be used to send information without affecting the V(S) or V(R) variables at either
     * station.. Reception of the UIH command/response is not sequence number verified by the data link procedures;
     * therefore, the UIH frame may be lost if a data link exception occurs during transmission of the protected portion of the
     * command, or duplicated if an exception condition occurs during any reply to the command. There is no specified
     * response to the UI command/response.
     * For the UI frame, the FCS shall be calculated over all fields (Address, Control, Length Indicator, Information).
     * Support of UI frames is optional.
     */
    UI = 0b00000011
};

/**
 * @brief Get the frame type string
 * 
 * @param frame_type 
 * @return const char* 
 */
const char *get_frame_type(enum CONTROL_TYPE frame_type);