using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;

namespace JInput {

    /// <summary>
    /// manager of all input going through the dll
    /// also deals with controllers and when there are new ones added
    /// </summary>
    public class InputManager : MonoBehaviour {

        #region DLL FUNCTIONS

        [DllImport("ControllerInputDll")]
        private static extern int startInput();

        [DllImport("ControllerInputDll")]
        private static extern void releaseInput();

        [DllImport("ControllerInputDll")]
        private static extern int updateInput();

        /// <summary>
        /// Checks for new controllers and updates the c++ list of controllers
        /// resets the input for all controllers if there is a new one added
        /// </summary>
        /// <returns>number of new controllers</returns>
        [DllImport("ControllerInputDll")]
        private static extern int updateControllers();

        [DllImport("ControllerInputDll")]
        private static extern void setCurrentController(int a_CurrentController);

        [DllImport("ControllerInputDll")]
        private static extern int getNumberOfControllers();


        //move to Controller script?
        [DllImport("ControllerInputDll")]
        private static extern IntPtr getPovName(int a_Direction);

        //move to Controller script?
        [DllImport("ControllerInputDll")]
        private static extern IntPtr getButtonNameConverted(int a_Button, bool a_IsXbox);

        #endregion


        private static InputManager m_Input;

        /// <summary>
        /// dead zone from the controller, if axis input is below this then it will be set to 0
        /// </summary>
        [Range(0.0f,1.0f)]
        public float m_DeadZone = 0.07f;

        [SerializeField]
        private List<Controller> m_Controllers = new List<Controller>();

        public static int numberOfControllers { get { return m_Input.m_Controllers.Count; } }


        #region FUNCTIONS

        //todo get last interacted with controller
        //todo check all controllers for input (check if any controller pressed X)

        //gets a controller
        public static Controller getController(int a_Index) {

            if (m_Input == null) {
                Debug.LogError("There is no InputManager in the scene!");
                return null;
            }

            if (a_Index >= m_Input.m_Controllers.Count || a_Index < 0) {
                //throw new Exception();
                return null;
            }
            return m_Input.m_Controllers[a_Index];
        }

        //move to Controller script?
        public static string GetNameFromHatDir(JInput.Controller a_Controller) {
            setCurrentController(a_Controller.controllerIndex);
            return Marshal.PtrToStringAnsi(getPovName(a_Controller.data.hatSwitch));
        }

        //move to Controller script?
        public static string GetNameFromButton(Controller a_Controller, int a_ButtonIndex) {
            setCurrentController(a_Controller.controllerIndex);
            return Marshal.PtrToStringAnsi(getButtonNameConverted(a_ButtonIndex, a_Controller.isXboxController));
        }

        public static int getNumOfControllers() {
            return numberOfControllers;
        }
        #endregion


        #region INTERNAL FUNCTIONS
        internal static float getControllerDeadZone() {
            return m_Input.m_DeadZone;
        }
        #endregion

        //these functions are used to interface with unity and to allow the input to update and deal with controllers

        #region MONOBEHAVIOUR FUNCTIONS

        // Update is called once per frame
        void Update() {
            addControllers();
            updateInput();

            for (int i = 0; i < m_Controllers.Count; i++) {
                m_Controllers[i].updateController();
            }
        }

        private void addControllers() {
            //calls c++ function which updates all controllers
            updateControllers();

            //reset
            for (int i = m_Controllers.Count; i < getNumberOfControllers(); i++) {
                Controller controller = new Controller();
                controller.startController(m_Controllers.Count);
                m_Controllers.Add(controller);
            }

        }

        private void OnDestroy() {
            if (this == m_Input) {
                releaseInput();
                m_Input = null;
            }
        }

        /// <summary>
        /// sets up the input for the first time on load
        /// </summary>
        void Awake() {
            SetupInput();
        }

        void OnEnable() {
            if (m_Input == null) {
                //should happen in awake,
                //will happen here if there is a code change and unity refreshes
                Debug.LogWarning("Input was null in Enable, something happen??");
                SetupInput();
            }
        }

        void OnDisable() {
            //if (this == m_Input) {
            //    Debug.LogWarning("Do not disable InputManager");
            //}
        }

        private void SetupInput() {
            if (m_Input != null) {
                Debug.LogError("There are two InputManager scripts in the scene, disabling " + transform.name + " object to stop the script.");
                enabled = false;
                gameObject.SetActive(false);
                return;
            }
            m_Input = this;
            DontDestroyOnLoad(m_Input);
            startInput();
            addControllers();
        }
        #endregion


    }
}