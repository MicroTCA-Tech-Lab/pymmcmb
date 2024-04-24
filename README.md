# Python wrapper for MMC mailbox

## Example usage

```
>>> import pymmcmb
>>> i = pymmcmb.get_mmc_information()
>>> i.board_name, i.amc_slot_nr, hex(i.ipmb_addr)
('DAMC-UNIZUP', 7, '0x7e')
>>> s = pymmcmb.get_fru_status(0)
>>> s.present, s.powered, s.compatible, s.failure, s.temperature
(True, True, True, False, [38.12, 39.43, 34.31, 41.37, 37.6, 44.36, 39.1])
>>> d = pymmcmb.get_fru_description(0)
>>> d.manufacturer, d.product, d.serial_nr, d.uid
('DESY', 'DAMC-UNIZUP', '0000', bytearray(b'\x04\x91b\xc0\xbc\x07'))
>>> sen = pymmcmb.get_mmc_sensors()
>>> [(s.name, s.reading) for s in sen[:4]]
[('STAMP Temp', 34.5), ('AMC MP 3V3', 3.339984655380249), ('AMC PP 12V', 12.354035377502441), ('I_RTM MP 3V3', 0.0)]
```

