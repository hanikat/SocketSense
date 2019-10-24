import Settings
import Motor
import Forces

mot = Motor.Motor()
mot.step_motor(22)

forces = Forces.Forces()
forces.load_forces(Settings.FORCE_VALUE_FILE)
