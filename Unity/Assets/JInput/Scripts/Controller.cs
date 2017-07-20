using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;

namespace JInput {

    /// <summary>
    /// states a button can be in relating to when it was interacted with
    /// </summary>
	internal enum ButtonStates {
		Normal,
		Pressed,
		Released
	}

    /// <summary>
    /// controller Information, every controller has one of these to store it's information
    /// </summary>
	[System.Serializable]
	public class ControllerData {
		public float[] axesValues = new float[6];
		public bool[] buttons = new bool[14];
		[HideInInspector]
		internal ButtonStates[] buttonStates = new ButtonStates[14];
		public int hatSwitch = 0;
	}

    /// <summary>
    /// list of axes on a controller
    /// </summary>
    public enum ControllerAxes {
        LStickX,
        LStickY,
        RStickX,
        RStickY,
        LeftTrigger,
        RightTrigger
    };

    /// <summary>
    /// axes that two axis associated with them
    /// </summary>
    public enum ControllerVec2Axes {
        LStick,
        RStick
    };

    //todo: Have a any button
    //(might just use a separate function that asks if any Button was pressed
    /// <summary>
    /// list of all buttons that are common on a controller
    /// </summary>
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

    /// <summary>
    /// directions relating to hat switch for the controller
    /// </summary>
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

    /// <summary>
    /// deals with information that relates to a single controller
    /// there can be many of these as each one links to a controller
    /// </summary>
	[System.Serializable]
	public class Controller {

        #region DLL FUNCTIONS

        /// <summary>
        /// Checks to see if a_Button is currently down
        /// </summary>
        /// <param name="a_Button">Button index to check</param>
        /// <returns>true if down</returns>
        [DllImport("ControllerInputDll")]
		private static extern int getButton(int a_Button);

        //[DllImport("ControllerInputDll")]
        //private static extern int getButtonDown(int a_Button);
        //
        //[DllImport("ControllerInputDll")]
        //private static extern int getButtonUp(int a_Button);

        /// <summary>
        /// gets controller information from a_Axis
        /// </summary>
        /// <param name="a_Axis">Which Axis to get data from</param>
        /// <returns>Value from a_Axis</returns>
        [DllImport("ControllerInputDll")]
		private static extern float getAxesValue(int a_Axis);

        /// <summary>
        /// gets the pov value right from the controller
        /// increments by 4500 each quarter
        /// 0 - up
        /// 4500 - up right
        /// 9000 - right
        /// </summary>
        /// <returns>-1 if not pressed, 0 < with direction</returns>
		[DllImport("ControllerInputDll")]
		private static extern int getPovValue();

        /// <summary>
        /// gets a direction between 0-8
        /// </summary>
        /// <returns>0 if not pressed,1-8 for direction</returns>
        [DllImport("ControllerInputDll")]
        private static extern int getPovDir();

        /// <summary>
        /// this might be useless in c#
        /// checks to see which type of controller this is based on it's GUID
        /// </summary>
        /// <returns>value that is based on a array in the C++ code</returns>
        [DllImport("ControllerInputDll")]
        private static extern int getJoystickType();

        /// <summary>
        /// checks to see if the controller is a xbox controller or not
        /// this is based off GUID, if the device is unknown it defaults to true
        /// </summary>
        /// <returns>is the controller a Xbox controller</returns>
		[DllImport("ControllerInputDll")]
		private static extern bool isControllerXbox();

        /// <summary>
        /// checks to see if the controller is active or not
        /// a inactive controller might have been disconnected or we cant connect to it
        /// </summary>
        /// <returns>is the controller active</returns>
		[DllImport("ControllerInputDll")]
		private static extern bool isControllerActive();

        /// <summary>
        /// sets this controller index as the current controller
        /// required to get information about this controller
        /// </summary>
        /// <param name="a_CurrentController">index of this controller</param>
		[DllImport("ControllerInputDll")]
        private static extern void setCurrentController(int a_CurrentController);
		#endregion

        /// <summary>
        /// data for this controller
        /// </summary>
		[SerializeField]
		private ControllerData m_Data = new ControllerData();

        /// <summary>
        /// a get property for controller data
        /// use functions to get data instead
        /// </summary>
		public ControllerData data { get { return m_Data; } }

        /// <summary>
        /// check to see if this controller is a Xbox controller
        /// </summary>
        private bool m_IsXbox = false;
        /// <summary>
        /// getter to check if this controller is a Xbox controller
        /// </summary>
		public bool isXboxController { get { return m_IsXbox; } }

        /// <summary>
        /// what is this controllers index in the array
        /// relates to index in InputManager
        /// </summary>
        private int m_ControllerIndex = 0;
        /// <summary>
        /// getter for this controllers Index in InputManager
        /// </summary>
		public int controllerIndex { get { return m_ControllerIndex; } }
        /// <summary>
        /// is this controller active and receiving input
        /// </summary>
		public bool m_IsActive = false;
        /// <summary>
        /// was this controller active last frame, this is done as a check to remove/reset any controller data when the controller is nolonger active
        /// </summary>
		private bool m_WasActive = true;


        /// <summary>
        /// Checks to see if a_Button is currently down
        /// </summary>
        /// <param name="a_Button">Button to check</param>
        /// <returns>true if down</returns>
        public bool IsButtonDown(ControllerButtons a_Button) {
			return m_Data.buttons[(int)a_Button];
		}

        /// <summary>
        /// Checks to see if a_Button was pressed last frame
        /// </summary>
        /// <param name="a_Button">Button to check</param>
        /// <returns>true if pressed last from</returns>
        public bool WasButtonPressed(ControllerButtons a_Button) {
			return m_Data.buttonStates[(int)a_Button] == ButtonStates.Pressed;
		}

        /// <summary>
        /// Checks to see if a_Button was released last frame
        /// </summary>
        /// <param name="a_Button">Button to check</param>
        /// <returns>true if released last frame</returns>
		public bool WasButtonReleased(ControllerButtons a_Button) {
			return m_Data.buttonStates[(int)a_Button] == ButtonStates.Released;
		}

        /// <summary>
        /// gets controller information from a_Axis
        /// </summary>
        /// <param name="a_Axes">Which Axis to get data from</param>
        /// <returns>Value from a_Axes</returns>
        public float getAxisValue(ControllerAxes a_Axes) {
            return m_Data.axesValues[(int)a_Axes];
        }

        /// <summary>
        /// Vector2 version of GetAxisValue
        /// </summary>
        /// <param name="a_Axes">Axis to get values for</param>
        /// <returns>Vector2 with axis values</returns>
        public Vector2 getAxisValue(ControllerVec2Axes a_Axes) {
            Vector2 axis = new Vector2();
            int axesIndex = (int)a_Axes;
            axis.x = m_Data.axesValues[axesIndex * 2 + 0];
            axis.y = m_Data.axesValues[axesIndex * 2 + 1];
            return axis;
        }

        /// <summary>
        /// returns a integer version of getHatDirection
        /// </summary>
        /// <returns>int between 0 and 8</returns>
        public int getPovDirection() {
			return m_Data.hatSwitch;
		}

		/// <summary>
		/// returns direction of the hat switch on the controller
		/// </summary>
		/// <returns>ControllerHatDir from direction</returns>
		public ControllerHatDir getHatDirection() {
			return (ControllerHatDir)m_Data.hatSwitch;
		}

		#region INTERNAL CONTROLLER FUNCTIONS

        /// <summary>
        /// starts up the controller
        /// sets up basic information for the controller
        /// </summary>
        /// <param name="a_ControllerIndex">index from controller array in InputManager</param>
		internal void startController(int a_ControllerIndex) {
			m_ControllerIndex = a_ControllerIndex;
			setCurrentController(m_ControllerIndex);
			m_IsXbox = isControllerXbox();
            m_Data = new ControllerData();
        }

        /// <summary>
        /// updates this controllers button and axes values
        /// also updates whether the buttons were pressed or released this frame
        /// </summary>
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

            //go through 14 buttons, should change to number of buttons on the controller
			for (int i = 0; i < 14; i++) {
				bool last = m_Data.buttons[i];
				m_Data.buttons[i] = getButton(i) >= 1;
                //if the button is not different then normal, else if the last one was down then released else pressed
                m_Data.buttonStates[i] = last != m_Data.buttons[i] ? (last ? ButtonStates.Released : ButtonStates.Pressed) : ButtonStates.Normal;
			}
			for (int i = 0; i < 6; i++) {
				m_Data.axesValues[i] = getAxesValue(i) / 1000.0f;
                //todo make from deadzone to 1 be 0 to 1
                //todo map axes info to a square??
                //dead zone
                float deadZone = InputManager.getControllerDeadZone();
                if(Mathf.Abs(m_Data.axesValues[i]) <= deadZone) {
                    m_Data.axesValues[i] = 0;
                }
			}
			m_Data.hatSwitch = getPovDir();
		}

		#endregion
	}
}