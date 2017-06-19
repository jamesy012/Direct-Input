using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;

namespace JInput {
	public class Input : MonoBehaviour {

		[DllImport("ControllerInputDll")]
		private static extern int startInput();

		[DllImport("ControllerInputDll")]
		private static extern void releaseInput();

		[DllImport("ControllerInputDll")]
		private static extern int updateInput();

		[DllImport("ControllerInputDll")]
		private static extern IntPtr getPovName(int a_Direction);

		[DllImport("ControllerInputDll")]
		private static extern IntPtr getButtonName(int a_Button);

		public static Input m_Input;

		[SerializeField]
		private List<Controller> m_Controllers = new List<Controller>();

		// Use this for initialization
		void Start() {
			if (m_Input != null) {
				Debug.LogError("THERE ARE TWO INPUT SCRIPTS");
				return;
			}
			m_Input = this;
			startInput();
			m_Controllers.Add(new Controller());//atm only one is available
		}

		// Update is called once per frame
		void Update() {
			updateInput();

			for (int i = 0; i < m_Controllers.Count; i++) {
				m_Controllers[i].updateController();
			}
		}

		public Controller getController(int a_Index) {
			return m_Controllers[a_Index];
		}

		public static string GetNameFromHatDir(JInput.Controller a_Controller) {
			return Marshal.PtrToStringAuto(getPovName(a_Controller.data.hatSwitch));
		}
		public static string GetNameFromButton(int a_ButtonIndex) {
			return Marshal.PtrToStringAuto(getButtonName(a_ButtonIndex));
		}
	}
}