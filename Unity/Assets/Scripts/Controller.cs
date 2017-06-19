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



		[SerializeField]
		private ControllerData m_Data = new ControllerData();

		public ControllerData data { get { return m_Data; } }

		public void updateController() {
			for (int i = 0; i < 14; i++) {
				m_Data.buttons[i] = getButton(i) >= 1;
			}
			for (int i = 0; i < 6; i++) {
				m_Data.axesValues[i] = getAxesValue(i);
			}
			m_Data.hatSwitch = getPovDir();
		}
	}
}