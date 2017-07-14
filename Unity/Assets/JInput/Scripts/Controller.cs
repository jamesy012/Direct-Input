using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;

namespace JInput {

	internal enum ButtonStates {
		Normal,
		Pressed,
		Released
	}

	[System.Serializable]
	public class ControllerData {
		public float[] axesValues = new float[6];
		public bool[] buttons = new bool[14];
		[HideInInspector]
		internal ButtonStates[] buttonStates = new ButtonStates[14];
		public int hatSwitch = 0;
	}

    public enum ControllerAxes {
        LStickX,
        LStickY,
        RStickX,
        RStickY,
        LeftTrigger,
        RightTrigger
    };

    public enum ControllerButtons {
        Cross = 0,
        A = 0,
        Circle = 1,
        B = 1,
        Square = 2,
        X = 2,
        Triangle = 3,
        Y = 3,
        L1 = 4,
        LB = 4,
        LeftBumper = 4,
        R1 = 5,
        RB = 5,
        RightBumper = 5,
        L3 = 6,
        LS = 6,
        LeftStick = 6,
        R3 = 7,
        RS = 7,
        RightStick = 7,
        Select = 8,
        Start = 9,
        Home = 10,
    };

	public enum ControllerHatDir {
		None,
		Up,
		UpRight,
		Right,
		DownRight,
		Down,
		DownLeft,
		Left,
		UpLeft
	}

	[System.Serializable]
	public class Controller {

		#region DLL FUNCTIONS
		[DllImport("ControllerInputDll")]
		private static extern int getButton(int a_Button);

		//[DllImport("ControllerInputDll")]
		//private static extern int getButtonDown(int a_Button);
		//
		//[DllImport("ControllerInputDll")]
		//private static extern int getButtonUp(int a_Button);

		[DllImport("ControllerInputDll")]
		private static extern float getAxesValue(int a_Axis);

		[DllImport("ControllerInputDll")]
		private static extern int getPovValue();

        [DllImport("ControllerInputDll")]
        private static extern int getPovDir();

        [DllImport("ControllerInputDll")]
        private static extern int getJoystickType();

		[DllImport("ControllerInputDll")]
		private static extern bool isControllerXbox();

		[DllImport("ControllerInputDll")]
		private static extern bool isControllerActive();

		[DllImport("ControllerInputDll")]
        private static extern void setCurrentController(int a_CurrentController);
		#endregion

		[SerializeField]
		private ControllerData m_Data = new ControllerData();

		public ControllerData data { get { return m_Data; } }

        private bool m_IsXbox = false;
		public bool isXboxController { get { return m_IsXbox; } }

        private int m_ControllerIndex = 0;
		public int controllerIndex { get { return m_ControllerIndex; } }
		public bool m_IsActive = false;
		private bool m_WasActive = true;

        

		public bool IsButtonDown(ControllerButtons a_Button) {
			return m_Data.buttons[(int)a_Button];
		}

		public bool WasButtonPressed(ControllerButtons a_Button) {
			return m_Data.buttonStates[(int)a_Button] == ButtonStates.Pressed;
		}

		public bool WasButtonReleased(ControllerButtons a_Button) {
			return m_Data.buttonStates[(int)a_Button] == ButtonStates.Released;
		}

		public float getAxisValue(ControllerAxes a_Axes) {
			return m_Data.axesValues[(int)a_Axes];
		}

		/// <summary>
		/// returns a integer version of getHatDirection
		/// </summary>
		/// <returns>int between 0 and 8</returns>
		public int getPovDirection() {
			return m_Data.hatSwitch;
		}

		/// <summary>
		/// returns direction of the hat swich on the controller
		/// </summary>
		/// <returns>ControllerHatDir from direction</returns>
		public ControllerHatDir getHatDirection() {
			return (ControllerHatDir)m_Data.hatSwitch;
		}

		#region INTERNAL CONTROLLER FUNCTIONS

		internal void startController(int a_ControllerIndex) {
			m_ControllerIndex = a_ControllerIndex;
			setCurrentController(m_ControllerIndex);
			m_IsXbox = isControllerXbox();
		}

		internal void updateController() {
			setCurrentController(m_ControllerIndex);
			m_IsActive = isControllerActive();
			if (!m_IsActive) {
				//small check to see if we should remove old data from the controller
				if (m_WasActive) {
					m_WasActive = false;
					//reset all controller information
					m_Data = new ControllerData();
				}
				return;
			}

			m_WasActive = true;

			for (int i = 0; i < 14; i++) {
				bool last = m_Data.buttons[i];
				m_Data.buttons[i] = getButton(i) >= 1;
				if (last != m_Data.buttons[i]) {//if there was a different state from this frame and the last
					switch (last) {
						case true:
							m_Data.buttonStates[i] = ButtonStates.Released;
							break;
						case false:
							m_Data.buttonStates[i] = ButtonStates.Pressed;
							break;
					}
				} else {
					m_Data.buttonStates[i] = ButtonStates.Normal;
				}
			}
			for (int i = 0; i < 6; i++) {
				m_Data.axesValues[i] = getAxesValue(i) / 1000.0f;
			}
			m_Data.hatSwitch = getPovDir();
		}

		#endregion
	}
}