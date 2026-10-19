# Opto lasers

Files to build and run a relatively cheap laser diode setup for optogenetics experiments.

| Path | Contents |
|---|---|
| [BOM.md](BOM.md) | Parts list |
| [laser_cage.png](laser_cage.png), `cage_assembly.FCStd` | Optical cage assembly |
| [Arduino/laser_control](Arduino/laser_control) | Arduino sketch that drives the laser ([usage](Arduino/README.md)) |

## Assembly

You need all parts in [BOM.md](BOM.md), an optical power meter and a plate to mount the cage assembly on.

### Optical cage

1. Insert the laser diode into the S1LM05.
2. Mount the S1LM05, with the diode in it, into the CP33/M.
3. Insert the SM1FC into the CXY1.
4. Screw the collimator into the CP1M09.
5. Assemble the components as shown in [laser_cage.png](laser_cage.png). Don't tighten all the screws yet, because you will adjust the distances later.

### Electronics

1. Wire the MAX5715 breakout board to the Arduino UNO
2. Connect one of the DAC outputs (OUTA–OUTD) to the analog input of the laser driver.
3. Connect the power supply, laser driver and laser diode. See the manuals of these components.
4. Connect the Arduino ground to the common ground of the PSU and driver.

### Software

1. Open [Arduino/laser_control](Arduino/laser_control) in the Arduino IDE and upload it to the Arduino UNO.
2. Open the serial monitor at **115200 baud**. See [Arduino/README.md](Arduino/README.md) for all commands.

## Testing and fine-tuning

> [!WARNING]
> Whenever you work with a powered laser diode, wear appropriate eye protection. Make sure no other people or animals are nearby.

1. Attach the optical fibre to the XY stage. Put the other end of the fibre into the power meter, and set the meter to the laser wavelength (462 nm).
2. Turn the blue potentiometer on the laser driver to its lowest setting.
3. Send `<h,1000>` in the serial monitor. This holds the DAC output at 1.0 V until you send another command. Use a higher value, up to `4095` (4.095 V), for more power.
4. Slowly turn up the blue potentiometer until the laser turns on.
5. Adjust the distances between the laser diode, collimator and fibre port to get the highest optical power. Then tighten all screws.
6. Adjust the X-Y position of the fibre port to get the highest power.
7. Send `<o>` to switch the laser off.
