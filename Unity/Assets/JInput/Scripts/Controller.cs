using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;

namespace JInput {

	[System.Serializable]
	public class ControllerData {
		public float[] axesValues = new float[6];
		public bool[] buttons = new bool[14];
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

    [System.Serializable]
	public class Controller {


		[DllImport("ControllerInputDll")]
		private static extern int getButton(int a_Button);

		[DllImport("ControllerInputDll")]
		private static extern int getAxesValue(int a_Axis);

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

        [SerializeField]
		private ControllerData m_Data = new ControllerData();

		public ControllerData data { get { return m_Data; } }

        public bool m_IsXbox = false;
        public int m_ControllerIndex = 0;
		public bool m_IsActive = false;

        public void startController(int a_ControllerIndex) {
            m_ControllerIndex = a_ControllerIndex;
            setCurrentController(m_ControllerIndex);
            m_IsXbox = isControllerXbox();
        }

		public void updateController() {
            setCurrentController(m_ControllerIndex);
			m_IsActive = isControllerActive();
			if (!m_IsActive) {
				return;
			}
            for (int i = 0; i < 14; i++) {
				m_Data.buttons[i] = getButton(i) >= 1;
			}
			for (int i = 0; i < 6; i++) {
				m_Data.axesValues[i] = getAxesValue(i) / 1000.0f;
			}
			m_Data.hatSwitch = getPovDir();
		}
        
        public bool IsButtonDown(ControllerButtons a_Button) {
            return m_Data.buttons[(int)a_Button];            
        }

	}
}