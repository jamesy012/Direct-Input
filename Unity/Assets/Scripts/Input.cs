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
		private static extern int updateControllers();

		[DllImport("ControllerInputDll")]
		private static extern IntPtr getPovName(int a_Direction);

        [DllImport("ControllerInputDll")]
        private static extern IntPtr getButtonNameConverted(int a_Button, bool a_IsXbox);

        [DllImport("ControllerInputDll")]
        private static extern void setCurrentController(int a_CurrentController);

        [DllImport("ControllerInputDll")]
        private static extern int getNumberOfControllers();

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
			addControllers();
		}

		// Update is called once per frame
		void Update() {
			addControllers();
			updateInput();

			for (int i = 0; i < m_Controllers.Count; i++) {
				m_Controllers[i].updateController();
			}
		}

		public Controller getController(int a_Index) {
            if(a_Index >= m_Controllers.Count || a_Index < 0) {
                //throw new Exception();
                return null;
            }
			return m_Controllers[a_Index];
		}

		public static string GetNameFromHatDir(JInput.Controller a_Controller) {
            setCurrentController(a_Controller.m_ControllerIndex);
			return Marshal.PtrToStringAnsi(getPovName(a_Controller.data.hatSwitch));
		}
		public static string GetNameFromButton(Controller a_Controller, int a_ButtonIndex) {
            setCurrentController(a_Controller.m_ControllerIndex);
            return Marshal.PtrToStringAnsi(getButtonNameConverted(a_ButtonIndex,a_Controller.m_IsXbox));
		}

		private void addControllers() {
			int newControllers = updateControllers();
			for(int i = 0; i < newControllers; i++) {
				Controller controller = new Controller();
				controller.startController(m_Controllers.Count);
				m_Controllers.Add(controller);
			}
		}
	}
}