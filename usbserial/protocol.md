DeskCandy USB Serial protocol
===

# Instruction format
Instructions are sent as ASCII encoded strings, `\n` terminated.  
Responses are sent as ASCII encoded strings, format is further specified in instruction documentation

# Instructions

## ? -- poke
For detecting if the attached device really _is_ a DeskCandy.  
_Since_: v1

**Instruction**: `?\n`  
**Response**: `!\n`

## v -- version
Gets the protocol version implemented by the DeskCandy  
_Since_: v1

**Instruction**: `v\n`
**Response**: `1\n` (For v1)

## q -- query
Gets the current configuration of the DeskCandy  
_Since_: v1

**Instruction**: `q\n`  
**Response**:  
(For a DeskCandy configured with 8 RGB LEDs, currently set to red)
```
number 8
format rgb
mode solid
solid ff 00 00
```

## n -- number
Sets number of LEDs
_Since_: v1

**Instruction**: `n 8\n` (to set 8 LEDs)  
**Response**: `ok\n`

## f -- format
Sets LED format. Supported formats:
 - `rgb` since v1

**Instruction**: `f rgb` (to set RGB format)
**Response**: `ok\n`

## m -- mode
<!-- Maybe don't do this and just implicitly change mode when sending a modedata command? -->
Sets LED colour mode. Supported modes:
 - `solid` since v1

**Instruction**: `m solid` (to set solid mode)
**Response**: `ok\n`

## solid -- single colour
Sets data for solid mode. Takes three hex bytes.
_Since_: v1

**Instruction** `solid ff 00 00\n` to set colour to red  
**Response**: `ok\n`

---

# Errors
If an error occurred with an instruction, the response will be of the format `e<error number>\n` (eg `e0` for an unrecognised instruction)

 - e0 -- unrecognised instruction
 - e1 -- incorrect parameter format
 - e2 -- not implemented

