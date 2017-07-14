using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;

namespace JInput {
	public class InputManager : MonoBehaviour {

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

        private static InputManager m_Input;

		[SerializeField]
		private List<Controller> m_Controllers = new List<Controller>();

        public static int numberOfControllers{ get { return m_Input.m_Controllers.Count; } }


		#region FUNCTIONS
		//gets a controller
		public static Controller getController(int a_Index) {
			if (a_Index >= m_Input.m_Controllers.Count || a_Index < 0) {
				//throw new Exception();
				return null;
			}
			return m_Input.m_Controllers[a_Index];
		}

		public static string GetNameFromHatDir(JInput.Controller a_Controller) {
			setCurrentController(a_Controller.controllerIndex);
			return Marshal.PtrToStringAnsi(getPovName(a_Controller.data.hatSwitch));
		}
		public static string GetNameFromButton(Controller a_Controller, int a_ButtonIndex) {
			setCurrentController(a_Controller.controllerIndex);
			return Marshal.PtrToStringAnsi(getButtonNameConverted(a_ButtonIndex, a_Controller.isXboxController));
		}

		public static int getNumOfControllers() {
			return numberOfControllers;
		}
		#endregion

		//these functions are used to interface with unity and to allow the input to update and deal with controllers

		#region MONOBEHAVIOUR FUNCTIONS
		// Use this for initialization
		void Start() {
			if (m_Input != null) {
				Debug.LogError("THERE ARE TWO INPUT SCRIPTS destroying " + transform.gameObject + " input script");
                Destroy(this);
				return;
			}
			m_Input = this;
            DontDestroyOnLoad(m_Input);
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

		private void addControllers() {
            //this breaks when a controller is disconnected and added
            //change the index to just be referencing what the dll has
			int newControllers = updateControllers();
            //if(getNumberOfControllers()!= m_Controllers.Count) {
                for (int i = m_Controllers.Count; i < getNumberOfControllers(); i++) {
                    Controller controller = new Controller();
                    controller.startController(m_Controllers.Count);
                    m_Controllers.Add(controller);
                }
            //}
			//for(int i = 0; i < newControllers; i++) {
			//	Controller controller = new Controller();
			//	controller.startController(m_Controllers.Count);
			//	m_Controllers.Add(controller);
			//}
		}

        private void OnDestroy() {
            releaseInput();
        }

		#endregion

	}
}