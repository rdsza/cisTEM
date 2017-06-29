#include "../core/gui_core_headers.h"

extern MyRefinementPackageAssetPanel *refinement_package_asset_panel;
extern MyRunProfilesPanel *run_profiles_panel;
extern MyVolumeAssetPanel *volume_asset_panel;
extern MyRefinementResultsPanel *refinement_results_panel;
extern MyMainFrame *main_frame;

wxDEFINE_EVENT(wxEVT_COMMAND_MASKERTHREAD_COMPLETED, wxThreadEvent);

AutoRefine3DPanel::AutoRefine3DPanel( wxWindow* parent )
:
AutoRefine3DPanelParent( parent )
{

	buffered_results = NULL;

	// Fill combo box..

	//FillGroupComboBox();

	my_job_id = -1;
	running_job = false;

//	group_combo_is_dirty = false;
//	run_profiles_are_dirty = false;

	SetInfo();
//	FillGroupComboBox();
//	FillRunProfileComboBox();

	wxSize input_size = InputSizer->GetMinSize();
	input_size.x += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
	input_size.y = -1;
	ExpertPanel->SetMinSize(input_size);
	ExpertPanel->SetSize(input_size);


	// set values //

	/*
	AmplitudeContrastNumericCtrl->SetMinMaxValue(0.0f, 1.0f);
	MinResNumericCtrl->SetMinMaxValue(0.0f, 50.0f);
	MaxResNumericCtrl->SetMinMaxValue(0.0f, 50.0f);
	DefocusStepNumericCtrl->SetMinMaxValue(1.0f, FLT_MAX);
	ToleratedAstigmatismNumericCtrl->SetMinMaxValue(0.0f, FLT_MAX);
	MinPhaseShiftNumericCtrl->SetMinMaxValue(-3.15, 3.15);
	MaxPhaseShiftNumericCtrl->SetMinMaxValue(-3.15, 3.15);
	PhaseShiftStepNumericCtrl->SetMinMaxValue(0.001, 3.15);

	result_bitmap.Create(1,1, 24);
	time_of_last_result_update = time(NULL);*/

	refinement_package_combo_is_dirty = false;
	run_profiles_are_dirty = false;
//	input_params_combo_is_dirty = false;
	selected_refinement_package = -1;

	RefinementPackageSelectPanel->AssetComboBox->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &AutoRefine3DPanel::OnRefinementPackageComboBox, this);
	Bind(wxEVT_COMMAND_MASKERTHREAD_COMPLETED, &AutoRefine3DPanel::OnMaskerThreadComplete, this);
	Bind(MY_ORTH_DRAW_EVENT, &AutoRefine3DPanel::OnOrthThreadComplete, this);

	my_refinement_manager.SetParent(this);

	FillRefinementPackagesComboBox();

	long time_of_last_result_update;


}

void AutoRefine3DPanel::SetInfo()
{

	wxLogNull *suppress_png_warnings = new wxLogNull;
	#include "icons/niko_picture1.cpp"
	wxBitmap niko_picture1_bmp = wxBITMAP_PNG_FROM_DATA(niko_picture1);

	#include "icons/niko_picture2.cpp"
	wxBitmap niko_picture2_bmp = wxBITMAP_PNG_FROM_DATA(niko_picture2);
	delete suppress_png_warnings;

	InfoText->GetCaret()->Hide();

	InfoText->BeginSuppressUndo();
	InfoText->BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
	InfoText->BeginBold();
	InfoText->BeginUnderline();
	InfoText->BeginFontSize(14);
	InfoText->WriteText(wxT("3D Auto Refinement"));
	InfoText->EndFontSize();
	InfoText->EndBold();
	InfoText->EndUnderline();
	InfoText->Newline();
	InfoText->Newline();
	InfoText->EndAlignment();







	InfoText->BeginAlignment(wxTEXT_ALIGNMENT_LEFT);
	InfoText->WriteText(wxT("This panel allows users to refine a 3D reconstruction to high resolution using Frealign (Grigorieff, 2016) without the need to set many of the parameters that are required for manual refinement (see Manual Refine panel). In the simplest case, all that is required is the specification of a refinement package (set up under Assets), a starting reference (for example, a reconstruction obtained from the ab-initio procedure) and an initial resolution limit used in the refinement. The resolution should start low, for at 30 Å, to remove potential bias in the starting reference. However, for particles that are close to spherical, such as apoferritin, a higher resolution should be specified, between 8 and 12 Å (see Expert Options)."));
	InfoText->Newline();
	InfoText->Newline();
	InfoText->EndAlignment();

	InfoText->BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
	InfoText->BeginBold();
	InfoText->BeginUnderline();
	InfoText->WriteText(wxT("Program Options"));
	InfoText->EndBold();
	InfoText->EndUnderline();
	InfoText->Newline();
	InfoText->Newline();
	InfoText->EndAlignment();

	InfoText->BeginAlignment(wxTEXT_ALIGNMENT_LEFT);
	InfoText->BeginBold();
	InfoText->WriteText(wxT("Starting Reference : "));
	InfoText->EndBold();
	InfoText->WriteText(wxT("The initial 3D reconstruction used to align particles against. This should be of reasonable quality to ensure successful refinement."));
	InfoText->Newline();
	InfoText->BeginBold();
	InfoText->WriteText(wxT("Initial Res. Limit (Å) : "));
	InfoText->EndBold();
	InfoText->WriteText(wxT("The starting resolution limit used to align particles against the starting reference. In most cases, this should specify a relatively low resolution to remove potential bias in the starting reference."));
	InfoText->Newline();
	InfoText->Newline();
	InfoText->EndAlignment();

	InfoText->BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
	InfoText->BeginBold();
	InfoText->BeginUnderline();
	InfoText->WriteText(wxT("Expert Options"));
	InfoText->EndBold();
	InfoText->EndUnderline();
	InfoText->Newline();
	InfoText->Newline();
	InfoText->EndAlignment();


	InfoText->BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
	InfoText->BeginUnderline();
	InfoText->WriteText(wxT("General Refinement"));
	InfoText->EndUnderline();
	InfoText->Newline();
	InfoText->Newline();
	InfoText->EndAlignment();









	InfoText->BeginBold();
	InfoText->WriteText(wxT("Low/High-Resolution Limit (Å) : "));
	InfoText->EndBold();
	InfoText->WriteText(wxT("The data used for refinement is usually bandpass-limited to exclude spurious low-resolution features in the particle background (set by the low-resolution limit) and high-resolution noise (set by the high-resolution limit). It is good practice to set the low-resolution limit to 2.5x the approximate particle mask radius. The high-resolution limit should remain significantly below the resolution of the reference used for refinement to enable unbiased resolution estimation using the Fourier Shell Correlation curve."));
	InfoText->Newline();

	InfoText->BeginBold();
	InfoText->WriteText(wxT("Inner/Outer Mask Radius (Å) : "));
	InfoText->EndBold();
	InfoText->WriteText(wxT("Radii describing a spherical mask with an inner and outer radius that will be applied to the final reconstruction and to the half reconstructions to calculate Fourier Shell Correlation curve. The inner radius is normally set to 0.0 but can assume non-zero values to remove density inside a particle if it represents largely disordered features, such as the genomic RNA or DNA of a virus."));
	InfoText->Newline();

	InfoText->BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
	InfoText->BeginUnderline();
	InfoText->WriteText(wxT("Global Search"));
	InfoText->EndUnderline();
	InfoText->Newline();
	InfoText->Newline();
	InfoText->EndAlignment();

	InfoText->BeginBold();
	InfoText->WriteText(wxT("Global Mask Radius (Å) : "));
	InfoText->EndBold();
	InfoText->WriteText(wxT("The radius describing the area within the boxed-out particles that contains the particles. This radius us usually larger than the particle radius to account for particles that are not perfectly centered. The best value will depend on the way the particles were picked."));
	InfoText->Newline();

	InfoText->BeginBold();
	InfoText->WriteText(wxT("Number of Results to Refine : "));
	InfoText->EndBold();
	InfoText->WriteText(wxT("For a global search, an angular grid search is performed and the alignment parameters for the N best matching projections are then refined further in a local refinement. Only the set of parameters yielding the best score (correlation coefficient) is kept. Increasing N will increase the chances of finding the correct particle orientations but will slow down the search. A value of 20 is recommended."));
	InfoText->Newline();

	InfoText->BeginBold();
	InfoText->WriteText(wxT("Search Range in X/Y (Å) : "));
	InfoText->EndBold();
	InfoText->WriteText(wxT("The global search can be limited in the X and Y directions (measured from the box center) to ensure that only particles close to the box center are found. This is useful when the particle density is high and particles end up close to each other. In this case, it is usually still possible to align all particles in a cluster of particles (assuming they do not significantly overlap). The values provided here for the search range should be set to exclude the possibility that the same particle is selected twice and counted as two different particles."));
	InfoText->Newline();


	InfoText->BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
	InfoText->BeginUnderline();
	InfoText->WriteText(wxT("Reconstruction"));
	InfoText->EndUnderline();
	InfoText->Newline();
	InfoText->Newline();
	InfoText->EndAlignment();

	InfoText->BeginBold();
	InfoText->WriteText(wxT("Autocrop Images? "));
	InfoText->EndBold();
	InfoText->WriteText(wxT("Should the particle images be cropped to a minimum size determined by the mask radius to accelerate 3D reconstruction? This is usually not recommended as it increases interpolation artifacts."));
	InfoText->Newline();

	InfoText->BeginBold();
	InfoText->WriteText(wxT("Apply Likelihood Blurring? "));
	InfoText->EndBold();
	InfoText->WriteText(wxT("Should the reconstructions be blurred by inserting each particle image at multiple orientations, weighted by a likelihood function? Enable this option if the ab-initio procedure appears to suffer from over-fitting and the appearance of spurious high-resolution features."));
	InfoText->Newline();

	InfoText->BeginBold();
	InfoText->WriteText(wxT("Smoothing Factor : "));
	InfoText->EndBold();
	InfoText->WriteText(wxT("A factor that reduces the range of likelihoods used for blurring. A smaller number leads to more blurring. The user should try values between 0.1 and 1."));
	InfoText->Newline();

	InfoText->BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
	InfoText->BeginUnderline();
	InfoText->WriteText(wxT("Masking"));
	InfoText->EndUnderline();
	InfoText->Newline();
	InfoText->Newline();
	InfoText->EndAlignment();

	InfoText->BeginBold();
	InfoText->WriteText(wxT("Use Auto-Masking? "));
	InfoText->EndBold();
	InfoText->WriteText(wxT("Should the 3D reconstructions be masked? Masking can suppress spurious density features that could be amplified during the iterative refinement. Masking should only be disabled if it appears to interfere with the reconstruction process."));
	InfoText->Newline();
	InfoText->Newline();

	InfoText->BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
	InfoText->BeginBold();
	InfoText->BeginUnderline();
	InfoText->WriteText(wxT("References"));
	InfoText->EndBold();
	InfoText->EndUnderline();
	InfoText->Newline();
	InfoText->Newline();
	InfoText->EndAlignment();

	InfoText->BeginAlignment(wxTEXT_ALIGNMENT_LEFT);
	InfoText->BeginBold();
	InfoText->WriteText(wxT("Grigorieff, N.,"));
	InfoText->EndBold();
	InfoText->WriteText(wxT(" 2016. Frealign: An exploratory tool for single-particle cryo-EM. Methods Enzymol. 579, 191-226. "));
	InfoText->BeginURL("http://dx.doi.org/10.1016/bs.mie.2016.04.013");
	InfoText->BeginUnderline();
	InfoText->BeginTextColour(*wxBLUE);
	InfoText->WriteText(wxT("doi:10.1016/bs.mie.2016.04.013"));
	InfoText->EndURL();
	InfoText->EndTextColour();
	InfoText->EndUnderline();
	InfoText->EndAlignment();
	InfoText->Newline();
	InfoText->Newline();

}

void AutoRefine3DPanel::OnInfoURL(wxTextUrlEvent& event)
{
	 const wxMouseEvent& ev = event.GetMouseEvent();

	 // filter out mouse moves, too many of them
	 if ( ev.Moving() ) return;

	 long start = event.GetURLStart();

	 wxTextAttr my_style;

	 InfoText->GetStyle(start, my_style);

	 // Launch the URL

	 wxLaunchDefaultBrowser(my_style.GetURL());
}


void AutoRefine3DPanel::ResetAllDefaultsClick( wxCommandEvent& event )
{
	// TODO : should probably check that the user hasn't changed the defaults yet in the future
	SetDefaults();
}

void AutoRefine3DPanel::SetDefaults()
{
	if (RefinementPackageSelectPanel->GetCount() > 0)
	{
		float calculated_high_resolution_cutoff;
		float local_mask_radius;
		float global_mask_radius;
		float search_range;

		ExpertPanel->Freeze();

		calculated_high_resolution_cutoff = 30.0;

		local_mask_radius = refinement_package_asset_panel->all_refinement_packages.Item(RefinementPackageSelectPanel->GetSelection()).estimated_particle_size_in_angstroms * 0.6;
		global_mask_radius = refinement_package_asset_panel->all_refinement_packages.Item(RefinementPackageSelectPanel->GetSelection()).estimated_particle_size_in_angstroms;

		search_range = refinement_package_asset_panel->all_refinement_packages.Item(RefinementPackageSelectPanel->GetSelection()).estimated_particle_size_in_angstroms * 0.5;

		// Set the values..

		float low_res_limit = refinement_package_asset_panel->all_refinement_packages.Item(RefinementPackageSelectPanel->GetSelection()).estimated_particle_size_in_angstroms * 1.5;
		if (low_res_limit > 300.00) low_res_limit = 300.00;

		LowResolutionLimitTextCtrl->SetValue(wxString::Format("%.2f", low_res_limit));
		HighResolutionLimitTextCtrl->SetValue(wxString::Format("%.2f", calculated_high_resolution_cutoff));
		MaskRadiusTextCtrl->SetValue(wxString::Format("%.2f", local_mask_radius));

		GlobalMaskRadiusTextCtrl->SetValue(wxString::Format("%.2f", global_mask_radius));
		NumberToRefineSpinCtrl->SetValue(20);
		SearchRangeXTextCtrl->SetValue(wxString::Format("%.2f", search_range));
		SearchRangeYTextCtrl->SetValue(wxString::Format("%.2f", search_range));

		InnerMaskRadiusTextCtrl->SetValue("0.00");

		AutoCropYesRadioButton->SetValue(false);
		AutoCropNoRadioButton->SetValue(true);

		ApplyBlurringNoRadioButton->SetValue(true);
		ApplyBlurringYesRadioButton->SetValue(false);
		SmoothingFactorTextCtrl->SetValue("1.00");

		AutoMaskYesRadio->SetValue(true);

		ExpertPanel->Thaw();
	}

}

void AutoRefine3DPanel::OnUpdateUI( wxUpdateUIEvent& event )
{
	// are there enough members in the selected group.
	if (main_frame->current_project.is_open == false)
	{
		ReferenceSelectPanel->Enable(false);
		RefinementPackageSelectPanel->Enable(false);
//		InputParametersComboBox->Enable(false);
		RefinementRunProfileComboBox->Enable(false);
		ReconstructionRunProfileComboBox->Enable(false);
		ExpertToggleButton->Enable(false);
		StartRefinementButton->Enable(false);
//		LocalRefinementRadio->Enable(false);
//		GlobalRefinementRadio->Enable(false);
//		NumberRoundsSpinCtrl->Enable(false);
//		UseMaskCheckBox->Enable(false);
//		MaskSelectPanel->Enable(false);


		if (ExpertPanel->IsShown() == true)
		{
			ExpertToggleButton->SetValue(false);
			ExpertPanel->Show(false);
			Layout();

		}

		if (RefinementPackageSelectPanel->GetCount() > 0)
		{
			RefinementPackageSelectPanel->Clear();
			RefinementPackageSelectPanel->ChangeValue("");
		}

		if (ReferenceSelectPanel->GetCount() > 0)
		{
			ReferenceSelectPanel->Clear();
			ReferenceSelectPanel->ChangeValue("");
		}
/*
		if (InputParametersComboBox->GetCount() > 0)
		{
			InputParametersComboBox->Clear();
			InputParametersComboBox->ChangeValue("");
		}*/

		if (ReconstructionRunProfileComboBox->GetCount() > 0)
		{
			ReconstructionRunProfileComboBox->Clear();
			ReconstructionRunProfileComboBox->ChangeValue("");
		}

		if (RefinementRunProfileComboBox->GetCount() > 0)
		{
			RefinementRunProfileComboBox->Clear();
			RefinementRunProfileComboBox->ChangeValue("");
		}

		if (PleaseCreateRefinementPackageText->IsShown())
		{
			PleaseCreateRefinementPackageText->Show(false);
			Layout();
		}

	}
	else
	{
		if (running_job == false)
		{
			if (ReferenceSelectPanel->GetCount() > 0) ReferenceSelectPanel->Enable(true);
			else ReferenceSelectPanel->Enable(false);

			RefinementRunProfileComboBox->Enable(true);
			ReconstructionRunProfileComboBox->Enable(true);
			InitialResLimitStaticText->Enable(true);
			HighResolutionLimitTextCtrl->Enable(true);

			ExpertToggleButton->Enable(true);

			if (RefinementPackageSelectPanel->GetCount() > 0)
			{
				RefinementPackageSelectPanel->Enable(true);
//				InputParametersComboBox->Enable(true);

				/*
				UseMaskCheckBox->Enable(true);

				if (UseMaskCheckBox->GetValue() == true)
				{
					MaskSelectPanel->Enable(true);
				}
				else
				{
					MaskSelectPanel->Enable(false);
					MaskSelectPanel->AssetComboBox->ChangeValue("");
				}
				*/

				if (PleaseCreateRefinementPackageText->IsShown())
				{
					PleaseCreateRefinementPackageText->Show(false);
					Layout();
				}

			}
			else
			{
//				UseMaskCheckBox->Enable(false);
	//			MaskSelectPanel->Enable(false);
	//			MaskSelectPanel->AssetComboBox->ChangeValue("");
				RefinementPackageSelectPanel->ChangeValue("");
				RefinementPackageSelectPanel->Enable(false);
//				InputParametersComboBox->ChangeValue("");
	//			InputParametersComboBox->Enable(false);

				if (PleaseCreateRefinementPackageText->IsShown() == false)
				{
					PleaseCreateRefinementPackageText->Show(true);
					Layout();
				}
			}

			if (ExpertToggleButton->GetValue() == true)
			{

				if (ApplyBlurringYesRadioButton->GetValue() == true)
				{
					SmoothingFactorTextCtrl->Enable(true);
					SmoothingFactorStaticText->Enable(true);
				}
				else
				{
					SmoothingFactorTextCtrl->Enable(false);
					SmoothingFactorStaticText->Enable(false);
				}
			}

			bool estimation_button_status = false;

			if (RefinementPackageSelectPanel->GetCount() > 0 && ReconstructionRunProfileComboBox->GetCount() > 0)
			{
				if (run_profiles_panel->run_profile_manager.ReturnTotalJobs(RefinementRunProfileComboBox->GetSelection()) > 1 && run_profiles_panel->run_profile_manager.ReturnTotalJobs(ReconstructionRunProfileComboBox->GetSelection()) > 1)
				{
					if (RefinementPackageSelectPanel->GetSelection() != wxNOT_FOUND && ReferenceSelectPanel->GetSelection() != wxNOT_FOUND)
					{
						//if (UseMaskCheckBox->GetValue() == false || MaskSelectPanel->AssetComboBox->GetSelection() != wxNOT_FOUND)
						estimation_button_status = true;
					}

				}
			}

			StartRefinementButton->Enable(estimation_button_status);

			if (refinement_package_combo_is_dirty == true)
			{
				FillRefinementPackagesComboBox();
				refinement_package_combo_is_dirty = false;
			}

			if (run_profiles_are_dirty == true)
			{
				FillRunProfileComboBoxes();
				run_profiles_are_dirty = false;
			}

			if (volumes_are_dirty == true)
			{
				ReferenceSelectPanel->FillComboBox();
				volumes_are_dirty = false;
			}
		}
		else
		{
			ReferenceSelectPanel->Enable(false);
			RefinementPackageSelectPanel->Enable(false);
			ExpertToggleButton->Enable(false);
			InitialResLimitStaticText->Enable(false);
			HighResolutionLimitTextCtrl->Enable(false);

			if (ExpertPanel->IsShown() == true)
			{
				ExpertToggleButton->SetValue(false);
				ExpertPanel->Show(false);
				Layout();
			}
		}



	}

}

void AutoRefine3DPanel::OnExpertOptionsToggle( wxCommandEvent& event )
{

	if (ExpertToggleButton->GetValue() == true)
	{
		ExpertPanel->Show(true);
		Layout();
	}
	else
	{
		ExpertPanel->Show(false);
		Layout();
	}
}

void AutoRefine3DPanel::FillRefinementPackagesComboBox()
{
	if (RefinementPackageSelectPanel->FillComboBox() == false) NewRefinementPackageSelected();
}

void AutoRefine3DPanel::NewRefinementPackageSelected()
{
	selected_refinement_package = RefinementPackageSelectPanel->GetSelection();
	SetDefaults();

	if (RefinementPackageSelectPanel->GetCount() > 0 && ReferenceSelectPanel->GetCount() > 0) ReferenceSelectPanel->SetSelection(volume_asset_panel->ReturnArrayPositionFromAssetID(refinement_package_asset_panel->all_refinement_packages.Item(RefinementPackageSelectPanel->GetSelection()).references_for_next_refinement[0]));
	//wxPrintf("New Refinement Package Selection\n");

}

void AutoRefine3DPanel::OnRefinementPackageComboBox( wxCommandEvent& event )
{

	NewRefinementPackageSelected();

}

void AutoRefine3DPanel::OnInputParametersComboBox( wxCommandEvent& event )
{
	//SetDefaults();
}

void AutoRefine3DPanel::TerminateButtonClick( wxCommandEvent& event )
{
	main_frame->job_controller.KillJob(my_job_id);

	WriteBlueText("Terminated Job");
	TimeRemainingText->SetLabel("Time Remaining : Terminated");
	CancelAlignmentButton->Show(false);
	FinishButton->Show(true);
	ProgressPanel->Layout();
/*
	if (buffered_results != NULL)
	{
		delete [] buffered_results;
		buffered_results = NULL;
	}*/
}

void AutoRefine3DPanel::FinishButtonClick( wxCommandEvent& event )
{
	ProgressBar->SetValue(0);
	TimeRemainingText->SetLabel("Time Remaining : ???h:??m:??s");
    CancelAlignmentButton->Show(true);
	FinishButton->Show(false);

	InputParamsPanel->Show(true);
	ProgressPanel->Show(false);
	StartPanel->Show(true);
	OutputTextPanel->Show(false);
	output_textctrl->Clear();
	ShowRefinementResultsPanel->Show(false);
	ShowRefinementResultsPanel->Clear();
	InfoPanel->Show(true);

	if (my_refinement_manager.output_refinement != NULL) delete my_refinement_manager.output_refinement;

	if (ExpertToggleButton->GetValue() == true) ExpertPanel->Show(true);
	else ExpertPanel->Show(false);
	running_job = false;
	Layout();

	//CTFResultsPanel->CTF2DResultsPanel->should_show = false;
	//CTFResultsPanel->CTF2DResultsPanel->Refresh();

}




void AutoRefine3DPanel::StartRefinementClick( wxCommandEvent& event )
{
	my_refinement_manager.BeginRefinementCycle();
}

void AutoRefine3DPanel::WriteInfoText(wxString text_to_write)
{
	output_textctrl->SetDefaultStyle(wxTextAttr(*wxBLACK));
	output_textctrl->AppendText(text_to_write);

	if (text_to_write.EndsWith("\n") == false)	 output_textctrl->AppendText("\n");
}

void AutoRefine3DPanel::WriteBlueText(wxString text_to_write)
{
	output_textctrl->SetDefaultStyle(wxTextAttr(*wxBLUE));
	output_textctrl->AppendText(text_to_write);

	if (text_to_write.EndsWith("\n") == false)	 output_textctrl->AppendText("\n");
}

void AutoRefine3DPanel::WriteErrorText(wxString text_to_write)
{
	 output_textctrl->SetDefaultStyle(wxTextAttr(*wxRED));
	 output_textctrl->AppendText(text_to_write);

	 if (text_to_write.EndsWith("\n") == false)	 output_textctrl->AppendText("\n");
}


void AutoRefine3DPanel::FillRunProfileComboBoxes()
{
	ReconstructionRunProfileComboBox->FillWithRunProfiles();
	RefinementRunProfileComboBox->FillWithRunProfiles();
}

void AutoRefine3DPanel::OnJobSocketEvent(wxSocketEvent& event)
{
      SETUP_SOCKET_CODES

	  wxString s = _("OnSocketEvent: ");
	  wxSocketBase *sock = event.GetSocket();
	  sock->SetFlags(wxSOCKET_BLOCK | wxSOCKET_WAITALL);

	//  MyDebugAssertTrue(sock == main_frame->job_controller.job_list[my_job_id].socket, "Socket event from Non conduit socket??");

	  // First, print a message
	  switch(event.GetSocketEvent())
	  {
	    case wxSOCKET_INPUT : s.Append(_("wxSOCKET_INPUT\n")); break;
	    case wxSOCKET_LOST  : s.Append(_("wxSOCKET_LOST\n")); break;
	    default             : s.Append(_("Unexpected event !\n")); break;
	  }

	  //m_text->AppendText(s);

	  //MyDebugPrint(s);

	  // Now we process the event
	  switch(event.GetSocketEvent())
	  {
	    case wxSOCKET_INPUT:
	    {
	      // We disable input events, so that the test doesn't trigger
	      // wxSocketEvent again.
	      sock->SetNotify(wxSOCKET_LOST_FLAG);
	      ReadFromSocket(sock, &socket_input_buffer, SOCKET_CODE_SIZE);


	      if (memcmp(socket_input_buffer, socket_send_job_details, SOCKET_CODE_SIZE) == 0) // identification
	      {
	    	  // send the job details..

	    	  //wxPrintf("Sending Job Details...\n");
	    	  my_job_package.SendJobPackage(sock);

	      }
	      else
	      if (memcmp(socket_input_buffer, socket_i_have_an_error, SOCKET_CODE_SIZE) == 0) // identification
	      {

	    	  wxString error_message;
   			  error_message = ReceivewxStringFromSocket(sock);

   			  WriteErrorText(error_message);
    	  }
	      else
	      if (memcmp(socket_input_buffer, socket_i_have_info, SOCKET_CODE_SIZE) == 0) // identification
	      {

	    	  wxString info_message;
   			  info_message = ReceivewxStringFromSocket(sock);

   			  WriteInfoText(info_message);
    	  }
	      else
	      if (memcmp(socket_input_buffer, socket_job_finished, SOCKET_CODE_SIZE) == 0) // identification
	 	  {
	 		 // which job is finished?

	 		 int finished_job;
	 		 ReadFromSocket(sock, &finished_job, 4);

	 		 my_job_tracker.MarkJobFinished();

//	 		 if (my_job_tracker.ShouldUpdate() == true) UpdateProgressBar();
	 		 //WriteInfoText(wxString::Format("Job %i has finished!", finished_job));
	 	  }
	      else
	      if (memcmp(socket_input_buffer, socket_job_result, SOCKET_CODE_SIZE) == 0) // identification
	 	  {
	    	  JobResult temp_result;
	    	  temp_result.ReceiveFromSocket(sock);

	    	  // send the result to the

	    	  my_refinement_manager.ProcessJobResult(&temp_result);
	    	  wxPrintf("Warning: Received socket_job_result - should this happen?");

	 	  }
	      else
	      if (memcmp(socket_input_buffer, socket_job_result_queue, SOCKET_CODE_SIZE) == 0) // identification
	 	  {
	    	  ArrayofJobResults temp_queue;
	    	  ReceiveResultQueueFromSocket(sock, temp_queue);

	    	  for (int counter = 0; counter < temp_queue.GetCount(); counter++)
	    	  {
	    		  my_refinement_manager.ProcessJobResult(&temp_queue.Item(counter));
	    	  }
	 	  }
	      else
		  if (memcmp(socket_input_buffer, socket_number_of_connections, SOCKET_CODE_SIZE) == 0) // identification
		  {
			  // how many connections are there?

			  int number_of_connections;
			  ReadFromSocket(sock, &number_of_connections, 4);

              my_job_tracker.AddConnection();

    //          if (graph_is_hidden == true) ProgressBar->Pulse();

              //WriteInfoText(wxString::Format("There are now %i connections\n", number_of_connections));

              // send the info to the gui

              int total_processes = my_job_package.my_profile.ReturnTotalJobs();
              if (my_job_package.number_of_jobs + 1 < my_job_package.my_profile.ReturnTotalJobs()) total_processes = my_job_package.number_of_jobs + 1;
              else total_processes =  my_job_package.my_profile.ReturnTotalJobs();


		 	  if (number_of_connections == total_processes) WriteInfoText(wxString::Format("All %i processes are connected.", number_of_connections));

			  if (length_of_process_number == 6) NumberConnectedText->SetLabel(wxString::Format("%6i / %6i processes connected.", number_of_connections, total_processes));
			  else
			  if (length_of_process_number == 5) NumberConnectedText->SetLabel(wxString::Format("%5i / %5i processes connected.", number_of_connections, total_processes));
		      else
			  if (length_of_process_number == 4) NumberConnectedText->SetLabel(wxString::Format("%4i / %4i processes connected.", number_of_connections, total_processes));
			  else
			  if (length_of_process_number == 3) NumberConnectedText->SetLabel(wxString::Format("%3i / %3i processes connected.", number_of_connections, total_processes));
			  else
			  if (length_of_process_number == 2) NumberConnectedText->SetLabel(wxString::Format("%2i / %2i processes connected.", number_of_connections, total_processes));
			  else
		      NumberConnectedText->SetLabel(wxString::Format("%1i / %1i processes connected.", number_of_connections, total_processes));
		  }
	      else
		  if (memcmp(socket_input_buffer, socket_all_jobs_finished, SOCKET_CODE_SIZE) == 0) // identification
		  {
			  my_refinement_manager.ProcessAllJobsFinished();
		  }

	      // Enable input events again.

	      sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);

	      break;
	    }


	    case wxSOCKET_LOST:
	    {

	    	//MyDebugPrint("Socket Disconnected!!\n");
	        sock->Destroy();
	        break;
	    }
	    default: ;
	  }

}


void AutoRefinementManager::SetParent(AutoRefine3DPanel *wanted_parent)
{
	my_parent = wanted_parent;
}

void AutoRefinementManager::BeginRefinementCycle()
{
	RefinementPackage *current_refinement_package = &refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection());

	current_refinement_package_asset_id = current_refinement_package->asset_id;

	int class_counter;
	long particle_counter;

	int number_of_classes = current_refinement_package->number_of_classes;
	long number_of_particles = current_refinement_package->contained_particles.GetCount();

	wxString blank_string = "";
	current_reference_filenames.Clear();
	current_reference_filenames.Add(blank_string, number_of_classes);

	// Clear scratch..

	global_delete_autorefine3d_scratch();


	// make sure we have references..
/*
	for (class_counter = 0; class_counter < refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).number_of_classes; class_counter++)
	{
		if (refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).references_for_next_refinement[class_counter] == -1)
		{
			my_parent->WriteErrorText("Error: A reference must be provided for auto refinement.\nEither Import one, create one with the ab-inito panel, or use manual refinement");
			return;

		}
	}
*/
	// setup input/output refinements

	long current_input_refinement_id = refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).refinement_ids[0];
	input_refinement = main_frame->current_project.database.GetRefinementByID(current_input_refinement_id);
	output_refinement = new Refinement;

	// create a refinement with random angles etc..

	output_refinement->SizeAndFillWithEmpty(number_of_particles, number_of_classes);
	output_refinement->refinement_id = main_frame->current_project.database.ReturnHighestRefinementID() + 1;


	// Randomise the input parameters, and set the default resolution statistics..

	class_high_res_limits.Clear();
	class_next_high_res_limits.Clear();

	for (class_counter = 0; class_counter < number_of_classes; class_counter++)
	{
		for ( particle_counter = 0; particle_counter < number_of_particles; particle_counter++)
		{
			if (number_of_classes == 1) input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].occupancy = 100.0;
			else input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].occupancy = 100.00 / input_refinement->number_of_classes;

			input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].phi = global_random_number_generator.GetUniformRandom() * 180.0;
			input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].theta = global_random_number_generator.GetUniformRandom() * 180.0;
			input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].psi = global_random_number_generator.GetUniformRandom() * 180.0;
			input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].xshift = 0;
			input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].yshift = 0;
			input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].score = 0.0;
			input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].image_is_active = 1;
			input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].sigma = 1.0;
		}

		input_refinement->class_refinement_results[class_counter].class_resolution_statistics.GenerateDefaultStatistics(refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).estimated_particle_weight_in_kda);

		class_high_res_limits.Add(my_parent->HighResolutionLimitTextCtrl->ReturnValue());
		class_next_high_res_limits.Add(my_parent->HighResolutionLimitTextCtrl->ReturnValue());
	}


	// how many particles to use..

	long number_of_asym_units = number_of_particles * ReturnNumberofAsymmetricUnits(current_refinement_package->symmetry);
	float estimated_required_asym_units = 8000.0f * expf(75.0f / powf(my_parent->HighResolutionLimitTextCtrl->ReturnValue(),2));
	long wanted_start_number_of_asym_units = myroundint(estimated_required_asym_units) * number_of_classes;

	// what percentage is this.

	start_percent_used = (float(wanted_start_number_of_asym_units) / float(number_of_asym_units)) * 100.0;
	if (start_percent_used > 100.0) start_percent_used = 100.0;

	current_percent_used = start_percent_used;
	max_percent_used = current_percent_used;

	this_is_the_final_round = false;
	number_of_rounds_run = 0;
	percent_used_per_round.Clear();
	resolution_per_round.Clear();
	high_res_limit_per_round.Clear();

	number_of_global_alignments.Clear();
	number_of_global_alignments.Add(0, number_of_particles);

	rounds_since_global_alignment.Clear();
	rounds_since_global_alignment.Add(0, number_of_particles);

	// we need to set the currently selected reference filenames..

	for (class_counter = 0; class_counter < number_of_classes; class_counter++)
	{
		current_reference_filenames.Item(class_counter) = volume_asset_panel->ReturnAssetLongFilename(my_parent->ReferenceSelectPanel->ReturnSelection());
	}

	// Do we need to do masking them?

	my_parent->Freeze();

	my_parent->InputParamsPanel->Show(false);
	my_parent->StartPanel->Show(false);
	my_parent->ProgressPanel->Show(true);
	my_parent->ExpertPanel->Show(false);
	my_parent->InfoPanel->Show(false);
	my_parent->OutputTextPanel->Show(true);
	my_parent->ShowRefinementResultsPanel->Clear();

	if (my_parent->ShowRefinementResultsPanel->LeftRightSplitter->IsSplit() == true) my_parent->ShowRefinementResultsPanel->LeftRightSplitter->Unsplit();
	if (my_parent->ShowRefinementResultsPanel->TopBottomSplitter->IsSplit() == true) my_parent->ShowRefinementResultsPanel->TopBottomSplitter->Unsplit();
	my_parent->ShowRefinementResultsPanel->Show(true);
	my_parent->Layout();

	my_parent->Thaw();

	if (my_parent->AutoMaskYesRadio->GetValue() == true)
	{
		DoMasking();
	}
	else
	{
		SetupRefinementJob();
		RunRefinementJob();
	}

}


void AutoRefinementManager::RunRefinementJob()
{
	running_job_type = REFINEMENT;
	number_of_received_particle_results = 0;

	output_refinement->SizeAndFillWithEmpty(input_refinement->number_of_particles, input_refinement->number_of_classes);
	//wxPrintf("Output refinement has %li particles and %i classes\n", output_refinement->number_of_particles, input_refinement->number_of_classes);
	current_output_refinement_id = main_frame->current_project.database.ReturnHighestRefinementID() + 1;

	output_refinement->refinement_id = current_output_refinement_id;

	output_refinement->name = wxString::Format("Auto #%li - Round %i", current_output_refinement_id, number_of_rounds_run + 1);

	output_refinement->refinement_was_imported_or_generated = false;
	output_refinement->datetime_of_run = wxDateTime::Now();
	output_refinement->starting_refinement_id = input_refinement->refinement_id;

	for (int class_counter = 0; class_counter < input_refinement->number_of_classes; class_counter++)
	{
		output_refinement->class_refinement_results[class_counter].low_resolution_limit = my_parent->LowResolutionLimitTextCtrl->ReturnValue();
		output_refinement->class_refinement_results[class_counter].high_resolution_limit = class_high_res_limits[class_counter];
		output_refinement->class_refinement_results[class_counter].mask_radius = my_parent->MaskRadiusTextCtrl->ReturnValue();
		output_refinement->class_refinement_results[class_counter].signed_cc_resolution_limit = 0;
		output_refinement->class_refinement_results[class_counter].global_resolution_limit = class_high_res_limits[class_counter];
		output_refinement->class_refinement_results[class_counter].global_mask_radius = my_parent->GlobalMaskRadiusTextCtrl->ReturnValue();
		output_refinement->class_refinement_results[class_counter].number_results_to_refine = my_parent->NumberToRefineSpinCtrl->GetValue();
		output_refinement->class_refinement_results[class_counter].angular_search_step = CalculateAngularStep(class_high_res_limits[class_counter], my_parent->MaskRadiusTextCtrl->ReturnValue());
		output_refinement->class_refinement_results[class_counter].search_range_x = my_parent->SearchRangeXTextCtrl->ReturnValue();
		output_refinement->class_refinement_results[class_counter].search_range_y = my_parent->SearchRangeYTextCtrl->ReturnValue();
		output_refinement->class_refinement_results[class_counter].classification_resolution_limit = 10.0;
		output_refinement->class_refinement_results[class_counter].should_focus_classify = false;
		output_refinement->class_refinement_results[class_counter].sphere_x_coord = 0;
		output_refinement->class_refinement_results[class_counter].sphere_y_coord = 0;
		output_refinement->class_refinement_results[class_counter].sphere_z_coord = 0;
		output_refinement->class_refinement_results[class_counter].should_refine_ctf = false;
		output_refinement->class_refinement_results[class_counter].defocus_search_range = 0;
		output_refinement->class_refinement_results[class_counter].defocus_search_step = 0;
	}

	output_refinement->percent_used = current_percent_used;

	output_refinement->resolution_statistics_box_size = input_refinement->resolution_statistics_box_size;
	output_refinement->resolution_statistics_pixel_size = input_refinement->resolution_statistics_pixel_size;

	// launch a controller

	current_job_starttime = time(NULL);

	time_of_last_update = current_job_starttime;
	my_parent->ShowRefinementResultsPanel->AngularPlotPanel->Clear();

	my_parent->WriteBlueText(wxString::Format(wxT("Running refinement round %2i (%.2f %%)\n"), number_of_rounds_run + 1, current_percent_used));

	for (int class_counter = 0; class_counter < input_refinement->number_of_classes; class_counter++)
	{
		my_parent->WriteBlueText(wxString::Format(wxT("Res. limit for class #%i = %.2f"), class_counter, class_high_res_limits[class_counter]));
	}

	current_job_id = main_frame->job_controller.AddJob(my_parent, run_profiles_panel->run_profile_manager.run_profiles[my_parent->RefinementRunProfileComboBox->GetSelection()].manager_command, run_profiles_panel->run_profile_manager.run_profiles[my_parent->RefinementRunProfileComboBox->GetSelection()].gui_address);
	my_parent->my_job_id = current_job_id;

	if (current_job_id != -1)
	{
		long number_of_refinement_processes;
	    if (my_parent->my_job_package.number_of_jobs + 1 < my_parent->my_job_package.my_profile.ReturnTotalJobs()) number_of_refinement_processes = my_parent->my_job_package.number_of_jobs + 1;
	    else number_of_refinement_processes =  my_parent->my_job_package.my_profile.ReturnTotalJobs();

		if (number_of_refinement_processes >= 100000) my_parent->length_of_process_number = 6;
		else
		if (number_of_refinement_processes >= 10000) my_parent->length_of_process_number = 5;
		else
		if (number_of_refinement_processes >= 1000) my_parent->length_of_process_number = 4;
		else
		if (number_of_refinement_processes >= 100) my_parent->length_of_process_number = 3;
		else
		if (number_of_refinement_processes >= 10) my_parent->length_of_process_number = 2;
		else
		my_parent->length_of_process_number = 1;

		if (my_parent->length_of_process_number == 6) my_parent->NumberConnectedText->SetLabel(wxString::Format("%6i / %6li processes connected.", 0, number_of_refinement_processes));
		else
		if (my_parent->length_of_process_number == 5) my_parent->NumberConnectedText->SetLabel(wxString::Format("%5i / %5li processes connected.", 0, number_of_refinement_processes));
		else
		if (my_parent->length_of_process_number == 4) my_parent->NumberConnectedText->SetLabel(wxString::Format("%4i / %4li processes connected.", 0, number_of_refinement_processes));
		else
		if (my_parent->length_of_process_number == 3) my_parent->NumberConnectedText->SetLabel(wxString::Format("%3i / %3li processes connected.", 0, number_of_refinement_processes));
		else
		if (my_parent->length_of_process_number == 2) my_parent->NumberConnectedText->SetLabel(wxString::Format("%2i / %2li processes connected.", 0, number_of_refinement_processes));
		else
		my_parent->NumberConnectedText->SetLabel(wxString::Format("%i / %li processes connected.", 0, number_of_refinement_processes));

		my_parent->TimeRemainingText->SetLabel("Time Remaining : ???h:??m:??s");
		my_parent->Layout();


		/*
		my_parent->StartPanel->Show(false);
		my_parent->ProgressPanel->Show(true);

		my_parent->ExpertPanel->Show(false);
		my_parent->InfoPanel->Show(false);
		my_parent->OutputTextPanel->Show(true);
 		my_parent->ShowRefinementResultsPanel->Show(true);

		my_parent->ExpertToggleButton->Enable(false);
		my_parent->RefinementPackageSelectPanel->Enable(false);
		*/

		my_parent->running_job = true;
		my_parent->my_job_tracker.StartTracking(my_parent->my_job_package.number_of_jobs);

	}




	my_parent->ProgressBar->Pulse();
}

void AutoRefinementManager::SetupMerge3dJob()
{
	int class_counter;

	my_parent->my_job_package.Reset(run_profiles_panel->run_profile_manager.run_profiles[my_parent->ReconstructionRunProfileComboBox->GetSelection()], "merge3d", refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).number_of_classes);

	for (class_counter = 0; class_counter < refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).number_of_classes; class_counter++)
	{
		wxString output_reconstruction_1			= "/dev/null";
		wxString output_reconstruction_2			= "/dev/null";
		wxString output_reconstruction_filtered		= main_frame->current_project.volume_asset_directory.GetFullPath() + wxString::Format("/volume_%li_%i.mrc", output_refinement->refinement_id, class_counter + 1);

		current_reference_filenames.Item(class_counter) = output_reconstruction_filtered;

		wxString output_resolution_statistics		= "/dev/null";
		float 	 molecular_mass_kDa					= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).estimated_particle_weight_in_kda;
		float    inner_mask_radius					= my_parent->InnerMaskRadiusTextCtrl->ReturnValue();
		float    outer_mask_radius					= my_parent->MaskRadiusTextCtrl->ReturnValue();
		wxString dump_file_seed_1 					= main_frame->ReturnAutoRefine3DScratchDirectory() + wxString::Format("dump_file_%li_%i_odd_.dump",  current_output_refinement_id, class_counter);
		wxString dump_file_seed_2 					= main_frame->ReturnAutoRefine3DScratchDirectory() + wxString::Format("dump_file_%li_%i_even_.dump", current_output_refinement_id, class_counter);

		my_parent->my_job_package.AddJob("ttttffftti",	output_reconstruction_1.ToUTF8().data(),
														output_reconstruction_2.ToUTF8().data(),
														output_reconstruction_filtered.ToUTF8().data(),
														output_resolution_statistics.ToUTF8().data(),
														molecular_mass_kDa, inner_mask_radius, outer_mask_radius,
														dump_file_seed_1.ToUTF8().data(),
														dump_file_seed_2.ToUTF8().data(),
														class_counter + 1);
	}
}



void AutoRefinementManager::RunMerge3dJob()
{
	running_job_type = MERGE;

	// start job..

	if (output_refinement->number_of_classes > 1) my_parent->WriteBlueText("Merging and Filtering Reconstructions...");
	else
	my_parent->WriteBlueText("Merging and Filtering Reconstruction...");

	current_job_id = main_frame->job_controller.AddJob(my_parent, run_profiles_panel->run_profile_manager.run_profiles[my_parent->ReconstructionRunProfileComboBox->GetSelection()].manager_command, run_profiles_panel->run_profile_manager.run_profiles[my_parent->ReconstructionRunProfileComboBox->GetSelection()].gui_address);
	my_parent->my_job_id = current_job_id;

	if (current_job_id != -1)
	{
		long number_of_refinement_processes;
	    if (my_parent->my_job_package.number_of_jobs + 1 < my_parent->my_job_package.my_profile.ReturnTotalJobs()) number_of_refinement_processes = my_parent->my_job_package.number_of_jobs + 1;
	    else number_of_refinement_processes =  my_parent->my_job_package.my_profile.ReturnTotalJobs();

		if (number_of_refinement_processes >= 100000) my_parent->length_of_process_number = 6;
		else
		if (number_of_refinement_processes >= 10000) my_parent->length_of_process_number = 5;
		else
		if (number_of_refinement_processes >= 1000) my_parent->length_of_process_number = 4;
		else
		if (number_of_refinement_processes >= 100) my_parent->length_of_process_number = 3;
		else
		if (number_of_refinement_processes >= 10) my_parent->length_of_process_number = 2;
		else
		my_parent->length_of_process_number = 1;

		if (my_parent->length_of_process_number == 6) my_parent->NumberConnectedText->SetLabel(wxString::Format("%6i / %6li processes connected.", 0, number_of_refinement_processes));
		else
		if (my_parent->length_of_process_number == 5) my_parent->NumberConnectedText->SetLabel(wxString::Format("%5i / %5li processes connected.", 0, number_of_refinement_processes));
		else
		if (my_parent->length_of_process_number == 4) my_parent->NumberConnectedText->SetLabel(wxString::Format("%4i / %4li processes connected.", 0, number_of_refinement_processes));
		else
		if (my_parent->length_of_process_number == 3) my_parent->NumberConnectedText->SetLabel(wxString::Format("%3i / %3li processes connected.", 0, number_of_refinement_processes));
		else
		if (my_parent->length_of_process_number == 2) my_parent->NumberConnectedText->SetLabel(wxString::Format("%2i / %2li processes connected.", 0, number_of_refinement_processes));
		else
		my_parent->NumberConnectedText->SetLabel(wxString::Format("%i / %li processes connected.", 0, number_of_refinement_processes));


	/*
		my_parent->StartPanel->Show(false);
		my_parent->ProgressPanel->Show(true);

		my_parent->ExpertPanel->Show(false);
		my_parent->InfoPanel->Show(false);
		my_parent->OutputTextPanel->Show(true);
			//	CTFResultsPanel->Show(true);

		my_parent->ExpertToggleButton->Enable(false);
		my_parent->RefinementPackageSelectPanel->Enable(false);
		*/


		my_parent->TimeRemainingText->SetLabel("Time Remaining : ???h:??m:??s");
		my_parent->Layout();
		my_parent->running_job = true;
		my_parent->my_job_tracker.StartTracking(my_parent->my_job_package.number_of_jobs);

		}

		my_parent->ProgressBar->Pulse();
}


void AutoRefinementManager::SetupReconstructionJob()
{
	wxArrayString written_parameter_files;

	// set sigmas based on resolution..

	for (int class_counter = 0; class_counter < output_refinement->number_of_classes; class_counter++)
	{
		for ( long particle_counter = 0; particle_counter < output_refinement->number_of_particles; particle_counter++)
        {
			if (class_high_res_limits[class_counter] > 10.0) output_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].sigma = 1.0;
        }
	}

	written_parameter_files = output_refinement->WriteFrealignParameterFiles(main_frame->current_project.parameter_file_directory.GetFullPath() + "/auto_output_par");

	int class_counter;
	long counter;
	int job_counter;
	long number_of_reconstruction_jobs;
	long number_of_reconstruction_processes;
	float current_particle_counter;

	long number_of_particles;
	float particles_per_job;

	// for now, number of jobs is number of processes -1 (master)..

	number_of_reconstruction_processes = run_profiles_panel->run_profile_manager.run_profiles[my_parent->ReconstructionRunProfileComboBox->GetSelection()].ReturnTotalJobs();
	number_of_reconstruction_jobs = number_of_reconstruction_processes - 1;

	number_of_particles = refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).contained_particles.GetCount();

	if (number_of_particles - number_of_reconstruction_jobs < number_of_reconstruction_jobs) particles_per_job = 1;
	particles_per_job = float(number_of_particles - number_of_reconstruction_jobs) / float(number_of_reconstruction_jobs);

	my_parent->my_job_package.Reset(run_profiles_panel->run_profile_manager.run_profiles[my_parent->ReconstructionRunProfileComboBox->GetSelection()], "reconstruct3d", number_of_reconstruction_jobs * refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).number_of_classes);

	for (class_counter = 0; class_counter < refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).number_of_classes; class_counter++)
	{
		current_particle_counter = 1.0;

		for (job_counter = 0; job_counter < number_of_reconstruction_jobs; job_counter++)
		{
			wxString input_particle_stack 		= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).stack_filename;
			wxString input_parameter_file 		= written_parameter_files[class_counter];
			wxString output_reconstruction_1    = "/dev/null";
			wxString output_reconstruction_2			= "/dev/null";
			wxString output_reconstruction_filtered		= "/dev/null";
			wxString output_resolution_statistics		= "/dev/null";
			wxString my_symmetry						= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).symmetry;

			long	 first_particle						= myroundint(current_particle_counter);

			current_particle_counter += particles_per_job;
			if (current_particle_counter > number_of_particles) current_particle_counter = number_of_particles;

			long	 last_particle						= myroundint(current_particle_counter);
			current_particle_counter+=1.0;

			float 	 pixel_size							= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).contained_particles[0].pixel_size;
			float    voltage_kV							= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).contained_particles[0].microscope_voltage;
			float 	 spherical_aberration_mm			= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).contained_particles[0].spherical_aberration;
			float    amplitude_contrast					= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).contained_particles[0].amplitude_contrast;
			float 	 molecular_mass_kDa					= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).estimated_particle_weight_in_kda;
			float    inner_mask_radius					= my_parent->InnerMaskRadiusTextCtrl->ReturnValue();
			float    outer_mask_radius					= my_parent->MaskRadiusTextCtrl->ReturnValue();
			float    resolution_limit_rec;

			if (this_is_the_final_round == true) resolution_limit_rec = 0;
			else
			resolution_limit_rec = input_refinement->class_refinement_results[class_counter].class_resolution_statistics.ReturnResolutionNShellsAfter(class_high_res_limits[class_counter], output_refinement->resolution_statistics_box_size / 10 );
			//wxPrintf("\n\n\n\nres limit = %.2f\n\n\n\n", resolution_limit_rec);

			float    score_weight_conversion;
			if (class_high_res_limits[class_counter] < 8) score_weight_conversion = 2;
			else score_weight_conversion = 0.0;


			float    score_threshold;
			if (current_percent_used * 3.0f < 100.0f) score_threshold = 0.333f; // we are refining 3 times more then current_percent_used, we want to use current percent used so it is always 1/3.
			else score_threshold = current_percent_used / 100.0; 	// now 3 times current_percent_used is more than 100%, we therefire refined them all, and so just take current_percent used

			bool	 adjust_scores						= true;//my_parent->AdjustScoreForDefocusYesRadio->GetValue();
			bool	 invert_contrast					= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).stack_has_white_protein;
			bool	 crop_images						= my_parent->AutoCropYesRadioButton->GetValue();
			bool	 dump_arrays						= true;
			wxString dump_file_1 						= main_frame->ReturnAutoRefine3DScratchDirectory() + wxString::Format("dump_file_%li_%i_odd_%i.dump", current_output_refinement_id, class_counter, job_counter +1);
			wxString dump_file_2 						= main_frame->ReturnAutoRefine3DScratchDirectory() + wxString::Format("dump_file_%li_%i_even_%i.dump", current_output_refinement_id, class_counter, job_counter + 1);

			wxString input_reconstruction;
			bool	 use_input_reconstruction;


			if (my_parent->ApplyBlurringYesRadioButton->GetValue() == true)
			{
				// do we have a reference..

				if (refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).references_for_next_refinement[class_counter] == -1)
				{
					input_reconstruction			= "/dev/null";
					use_input_reconstruction		= false;
				}
				else
				{
					input_reconstruction = current_reference_filenames.Item(class_counter);//volume_asset_panel->ReturnAssetLongFilename(volume_asset_panel->ReturnArrayPositionFromAssetID(refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).references_for_next_refinement[class_counter]));
					use_input_reconstruction = true;
				}


			}
			else
			{
				input_reconstruction			= "/dev/null";
				use_input_reconstruction		= false;
			}

			float    resolution_limit_ref               = my_parent->HighResolutionLimitTextCtrl->ReturnValue();
			float	 smoothing_factor					= my_parent->SmoothingFactorTextCtrl->ReturnValue();
			float    padding							= 1.0f;
			bool	 normalize_particles				= true;
			bool	 exclude_blank_edges				= false;
			bool	 split_even_odd						= false;
			bool     centre_mass                        = true;

			bool threshold_input_3d = true;

			my_parent->my_job_package.AddJob("ttttttttiifffffffffffffbbbbbbbbbbtt",
																		input_particle_stack.ToUTF8().data(),
																		input_parameter_file.ToUTF8().data(),
																		input_reconstruction.ToUTF8().data(),
																		output_reconstruction_1.ToUTF8().data(),
																		output_reconstruction_2.ToUTF8().data(),
																		output_reconstruction_filtered.ToUTF8().data(),
																		output_resolution_statistics.ToUTF8().data(),
																		my_symmetry.ToUTF8().data(),
																		first_particle,
																		last_particle,
																		pixel_size,
																		voltage_kV,
																		spherical_aberration_mm,
																		amplitude_contrast,
																		molecular_mass_kDa,
																		inner_mask_radius,
																		outer_mask_radius,
																		resolution_limit_rec,
																		resolution_limit_ref,
																		score_weight_conversion,
																		score_threshold,
																		smoothing_factor,
																		padding,
																		normalize_particles,
																		adjust_scores,
																		invert_contrast,
																		exclude_blank_edges,
																		crop_images,
																		split_even_odd,
																		centre_mass,
																		use_input_reconstruction,
																		threshold_input_3d,
																		dump_arrays,
																		dump_file_1.ToUTF8().data(),
																		dump_file_2.ToUTF8().data());




		}
	}
}


// for now we take the paramter

void AutoRefinementManager::RunReconstructionJob()
{
	running_job_type = RECONSTRUCTION;
	number_of_received_particle_results = 0;

	// in the future store the reconstruction parameters..

	// empty scratch directory..

//	if (wxDir::Exists(main_frame->current_project.scratch_directory.GetFullPath() + "/Refine3D/") == true) wxFileName::Rmdir(main_frame->current_project.scratch_directory.GetFullPath() + "/Refine3D/", wxPATH_RMDIR_RECURSIVE);
//	if (wxDir::Exists(main_frame->current_project.scratch_directory.GetFullPath() + "/Refine3D/") == false) wxFileName::Mkdir(main_frame->current_project.scratch_directory.GetFullPath() + "/Refine3D/");

	// launch a controller


	if (output_refinement->number_of_classes > 1) my_parent->WriteBlueText("Calculating Reconstructions...");
	else my_parent->WriteBlueText("Calculating Reconstruction...");

	current_job_id = main_frame->job_controller.AddJob(my_parent, run_profiles_panel->run_profile_manager.run_profiles[my_parent->ReconstructionRunProfileComboBox->GetSelection()].manager_command, run_profiles_panel->run_profile_manager.run_profiles[my_parent->ReconstructionRunProfileComboBox->GetSelection()].gui_address);
	my_parent->my_job_id = current_job_id;

	if (current_job_id != -1)
	{
		long number_of_refinement_processes;
	    if (my_parent->my_job_package.number_of_jobs + 1 < my_parent->my_job_package.my_profile.ReturnTotalJobs()) number_of_refinement_processes = my_parent->my_job_package.number_of_jobs + 1;
	    else number_of_refinement_processes =  my_parent->my_job_package.my_profile.ReturnTotalJobs();

		if (number_of_refinement_processes >= 100000) my_parent->length_of_process_number = 6;
		else
		if (number_of_refinement_processes >= 10000) my_parent->length_of_process_number = 5;
		else
		if (number_of_refinement_processes >= 1000) my_parent->length_of_process_number = 4;
		else
		if (number_of_refinement_processes >= 100) my_parent->length_of_process_number = 3;
		else
		if (number_of_refinement_processes >= 10) my_parent->length_of_process_number = 2;
		else
		my_parent->length_of_process_number = 1;

		if (my_parent->length_of_process_number == 6) my_parent->NumberConnectedText->SetLabel(wxString::Format("%6i / %6li processes connected.", 0, number_of_refinement_processes));
		else
		if (my_parent->length_of_process_number == 5) my_parent->NumberConnectedText->SetLabel(wxString::Format("%5i / %5li processes connected.", 0, number_of_refinement_processes));
		else
		if (my_parent->length_of_process_number == 4) my_parent->NumberConnectedText->SetLabel(wxString::Format("%4i / %4li processes connected.", 0, number_of_refinement_processes));
		else
		if (my_parent->length_of_process_number == 3) my_parent->NumberConnectedText->SetLabel(wxString::Format("%3i / %3li processes connected.", 0, number_of_refinement_processes));
		else
		if (my_parent->length_of_process_number == 2) my_parent->NumberConnectedText->SetLabel(wxString::Format("%2i / %2li processes connected.", 0, number_of_refinement_processes));



/*
		my_parent->StartPanel->Show(false);
		my_parent->ProgressPanel->Show(true);

		my_parent->ExpertPanel->Show(false);
		my_parent->InfoPanel->Show(false);
		my_parent->OutputTextPanel->Show(true);
			//	CTFResultsPanel->Show(true);

		my_parent->ExpertToggleButton->Enable(false);
		my_parent->RefinementPackageSelectPanel->Enable(false);
		*/

		my_parent->NumberConnectedText->SetLabel(wxString::Format("%i / %li processes connected.", 0, number_of_refinement_processes));
		my_parent->TimeRemainingText->SetLabel("Time Remaining : ???h:??m:??s");
		my_parent->Layout();
		my_parent->running_job = true;
		my_parent->my_job_tracker.StartTracking(my_parent->my_job_package.number_of_jobs);

	}
		my_parent->ProgressBar->Pulse();
}

void AutoRefinementManager::SetupRefinementJob()
{
	int class_counter;
	long counter;
	long number_of_refinement_jobs;
	int number_of_refinement_processes;
	float current_particle_counter;

	long number_of_particles;
	float particles_per_job;
	float likelihood_to_global;
	bool do_global_for_this_particle;

	// get the last refinement for the currently selected refinement package..

	wxArrayString written_parameter_files;
	wxArrayString written_res_files;

	float lowest_res = FLT_MAX;

	for (class_counter = 0; class_counter < input_refinement->number_of_classes; class_counter++)
	{
		lowest_res = std::min(class_high_res_limits[class_counter], lowest_res);
	}

	// setup whether to do global or local refinement..

	for ( long particle_counter = 0; particle_counter < input_refinement->number_of_particles; particle_counter++)
    {
		// should we do local or global?

		float round_adjust = powf(number_of_global_alignments[particle_counter] - floor(rounds_since_global_alignment[particle_counter] / 3), 2);
		if (round_adjust < 1) round_adjust = 1;

		float likelihood_to_global;
		if (lowest_res < 4) likelihood_to_global = -5; // they will all be local at such high res.
		else likelihood_to_global = powf(lowest_res, 2) / (250.0f * round_adjust); // very arbritrary

		if (fabsf(global_random_number_generator.GetUniformRandom()) < likelihood_to_global) do_global_for_this_particle = true;
		else do_global_for_this_particle = false;

		for (int class_counter = 0; class_counter < input_refinement->number_of_classes; class_counter++)
		{
			if (number_of_global_alignments[particle_counter] == 0) input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].image_is_active = 0.0;
			else
			{

				if (this_is_the_final_round == true) input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].image_is_active = 1.0;
				else
				if (rounds_since_global_alignment[particle_counter] == 0) input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].image_is_active = 1.0;
				else
				if (do_global_for_this_particle == true)
				{
					input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].image_is_active = 0.0;
				}
				else
				input_refinement->class_refinement_results[class_counter].particle_refinement_results[particle_counter].image_is_active = 1.0;
			}
        }
	}

	written_parameter_files = input_refinement->WriteFrealignParameterFiles(main_frame->current_project.parameter_file_directory.GetFullPath() + "/auto_input_par");
	written_res_files = input_refinement->WriteResolutionStatistics(main_frame->current_project.parameter_file_directory.GetFullPath() + "/auto_input_stats");

	// for now, number of jobs is number of processes -1 (master)..

	number_of_refinement_processes = run_profiles_panel->run_profile_manager.run_profiles[my_parent->RefinementRunProfileComboBox->GetSelection()].ReturnTotalJobs();
	number_of_refinement_jobs = number_of_refinement_processes - 1;

	number_of_particles = refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).contained_particles.GetCount();
	if (number_of_particles - number_of_refinement_jobs < number_of_refinement_jobs) particles_per_job = 1;
	else particles_per_job = float(number_of_particles - number_of_refinement_jobs) / float(number_of_refinement_jobs);

	my_parent->my_job_package.Reset(run_profiles_panel->run_profile_manager.run_profiles[my_parent->RefinementRunProfileComboBox->GetSelection()], "refine3d", number_of_refinement_jobs * refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).number_of_classes);

	for (class_counter = 0; class_counter < refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).number_of_classes; class_counter++)
	{
		current_particle_counter = 1;

		for (counter = 0; counter < number_of_refinement_jobs; counter++)
		{

			wxString input_particle_images					= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).stack_filename;
			wxString input_parameter_file 					= written_parameter_files.Item(class_counter);
			wxString input_reconstruction					= current_reference_filenames.Item(class_counter);
			wxString input_reconstruction_statistics 		= written_res_files.Item(class_counter);
			bool	 use_statistics							= true;

			wxString ouput_matching_projections		 		= "";
			wxString ouput_shift_file						= "/dev/null";

			wxString my_symmetry							= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).symmetry;
			long	 first_particle							= myroundint(current_particle_counter);

			current_particle_counter += particles_per_job;
			if (current_particle_counter > number_of_particles) current_particle_counter = number_of_particles;

			long	 last_particle							= myroundint(current_particle_counter);
			current_particle_counter++;

			float	 percent_used;
			if (number_of_rounds_run == 0) percent_used = 1.0f;
			else percent_used = (current_percent_used * 3.0) / 100.0;
			if (percent_used > 1) percent_used = 1;

#ifdef DEBUG
			wxString output_parameter_file = wxString::Format("/tmp/output_par_%li_%li.par", first_particle, last_particle);
#else
			wxString output_parameter_file = "/dev/null";
#endif

			// for now we take the paramters of the first image!!!!

			float 	 pixel_size								= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).contained_particles[0].pixel_size;
			float    voltage_kV								= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).contained_particles[0].microscope_voltage;
			float 	 spherical_aberration_mm				= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).contained_particles[0].spherical_aberration;
			float    amplitude_contrast						= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).contained_particles[0].amplitude_contrast;
			float	 molecular_mass_kDa						= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).estimated_particle_weight_in_kda;
			float    mask_radius							= my_parent->MaskRadiusTextCtrl->ReturnValue();
			float    inner_mask_radius						= my_parent->InnerMaskRadiusTextCtrl->ReturnValue();
			float    low_resolution_limit					= my_parent->LowResolutionLimitTextCtrl->ReturnValue();
			float    high_resolution_limit					= class_high_res_limits[class_counter];
			float	 signed_CC_limit						= 0;//my_parent->SignedCCResolutionTextCtrl->ReturnValue();
			float	 classification_resolution_limit		= 10.0;//class_high_res_limits[class_counter]; //my_parent->ClassificationHighResLimitTextCtrl->ReturnValue();
			float    mask_radius_search						= my_parent->GlobalMaskRadiusTextCtrl->ReturnValue();
			float	 high_resolution_limit_search			= class_high_res_limits[class_counter];
			float	 angular_step							= std::max(CalculateAngularStep(class_high_res_limits.Item(class_counter), my_parent->MaskRadiusTextCtrl->ReturnValue()), CalculateAngularStep(8.0, my_parent->MaskRadiusTextCtrl->ReturnValue()));
			int		 best_parameters_to_keep				= my_parent->NumberToRefineSpinCtrl->GetValue();
			float	 max_search_x							= my_parent->SearchRangeXTextCtrl->ReturnValue();
			float	 max_search_y							= my_parent->SearchRangeYTextCtrl->ReturnValue();
			float    mask_center_2d_x						= 0;//my_parent->SphereXTextCtrl->ReturnValue();
			float 	 mask_center_2d_y						= 0;//my_parent->SphereYTextCtrl->ReturnValue();
			float    mask_center_2d_z						= 0;//my_parent->SphereZTextCtrl->ReturnValue();
			float    mask_radius_2d							= 0;//my_parent->SphereRadiusTextCtrl->ReturnValue();

			float	 defocus_search_range					= 0;//my_parent->DefocusSearchRangeTextCtrl->ReturnValue();
			float	 defocus_step							= 0;//my_parent->DefocusSearchStepTextCtrl->ReturnValue();
			float	 padding								= 1.0;

			bool global_search = false;
			bool local_refinement = false;
			bool global_local_refinement = true;

			/*
			if (number_of_rounds_run == 0)
			{
				global_search = true;
				local_refinement = false;
			}
			else
			{
				global_search = false;
				local_refinement = true;

			}*/
			/*
			if (my_parent->GlobalRefinementRadio->GetValue() == true)
			{
				global_search = true;
				local_refinement = false;
			}
			else
			{
				global_search = false;
				local_refinement = true;
			}*/


			bool refine_psi 								= true; //my_parent->RefinePsiCheckBox->GetValue();
			bool refine_theta								= true; //my_parent->RefineThetaCheckBox->GetValue();
			bool refine_phi									= true; //my_parent->RefinePhiCheckBox->GetValue();
			bool refine_x_shift								= true; //my_parent->RefineXShiftCheckBox->GetValue();
			bool refine_y_shift								= true; //my_parent->RefineYShiftCheckBox->GetValue();
			bool calculate_matching_projections				= false;
			bool apply_2d_masking							= false; //my_parent->SphereClassificatonYesRadio->GetValue();
			bool ctf_refinement								= false;
			bool invert_contrast							= refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).stack_has_white_protein;

			bool normalize_particles = true;
			bool exclude_blank_edges = false;
			bool normalize_input_3d;

			if (my_parent->ApplyBlurringYesRadioButton->GetValue() == true) normalize_input_3d = false;
			else normalize_input_3d = true;

			bool threshold_input_3d = true;
			my_parent->my_job_package.AddJob("ttttbttttiifffffffffffffffifffffffffbbbbbbbbbbbbbbbbi",
																											input_particle_images.ToUTF8().data(),
																											input_parameter_file.ToUTF8().data(),
																											input_reconstruction.ToUTF8().data(),
																											input_reconstruction_statistics.ToUTF8().data(),
																											use_statistics,
																											ouput_matching_projections.ToUTF8().data(),
																											output_parameter_file.ToUTF8().data(),
																											ouput_shift_file.ToUTF8().data(),
																											my_symmetry.ToUTF8().data(),
																											first_particle,
																											last_particle,
																											percent_used,
																											pixel_size,
																											voltage_kV,
																											spherical_aberration_mm,
																											amplitude_contrast,
																											molecular_mass_kDa,
																											inner_mask_radius,
																											mask_radius,
																											low_resolution_limit,
																											high_resolution_limit,
																											signed_CC_limit,
																											classification_resolution_limit,
																											mask_radius_search,
																											high_resolution_limit_search,
																											angular_step,
																											best_parameters_to_keep,
																											max_search_x,
																											max_search_y,
																											mask_center_2d_x,
																											mask_center_2d_y,
																											mask_center_2d_z,
																											mask_radius_2d,
																											defocus_search_range,
																											defocus_step,
																											padding,
																											global_search,
																											local_refinement,
																											refine_psi,
																											refine_theta,
																											refine_phi,
																											refine_x_shift,
																											refine_y_shift,
																											calculate_matching_projections,
																											apply_2d_masking,
																											ctf_refinement,
																											normalize_particles,
																											invert_contrast,
																											exclude_blank_edges,
																											normalize_input_3d,
																											threshold_input_3d,
																											global_local_refinement,
																											class_counter);


		}

	}
}

void AutoRefinementManager::ProcessJobResult(JobResult *result_to_process)
{
	if (running_job_type == REFINEMENT)
	{

		int current_class = int(result_to_process->result_data[0] + 0.5);
		long current_particle = long(result_to_process->result_data[1] + 0.5) - 1;

		MyDebugAssertTrue(current_particle != -1 && current_class != -1, "Current Particle (%li) or Current Class(%i) = -1!", current_particle, current_class);

	//	wxPrintf("Received a refinement result for class #%i, particle %li\n", current_class + 1, current_particle + 1);
		//wxPrintf("output refinement has %i classes and %li particles\n", output_refinement->number_of_classes, output_refinement->number_of_particles);


		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].position_in_stack = long(result_to_process->result_data[1] + 0.5);
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].psi = result_to_process->result_data[2];
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].theta = result_to_process->result_data[3];
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].phi = result_to_process->result_data[4];
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].xshift = result_to_process->result_data[5];
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].yshift = result_to_process->result_data[6];
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].defocus1 = result_to_process->result_data[9];
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].defocus2 = result_to_process->result_data[10];
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].defocus_angle = result_to_process->result_data[11];
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].phase_shift = result_to_process->result_data[12];
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].occupancy = result_to_process->result_data[13];
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].logp = result_to_process->result_data[14];
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].sigma = result_to_process->result_data[15];
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].score = result_to_process->result_data[16];
		output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle].image_is_active = int(result_to_process->result_data[8]);

		number_of_received_particle_results++;
		//wxPrintf("received result!\n");
		long current_time = time(NULL);

		if (number_of_received_particle_results == 1)
		{
			current_job_starttime = current_time;
			time_of_last_update = 0;
			my_parent->ShowRefinementResultsPanel->AngularPlotPanel->SetSymmetryAndNumber(refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection()).symmetry,output_refinement->number_of_particles);
			my_parent->Layout();
		}
		else
		if (current_time != time_of_last_update)
		{
			int current_percentage = float(number_of_received_particle_results) / float(output_refinement->number_of_particles * output_refinement->number_of_classes) * 100.0;
			time_of_last_update = current_time;
			if (current_percentage > 100) current_percentage = 100;
			my_parent->ProgressBar->SetValue(current_percentage);
			long job_time = current_time - current_job_starttime;
			float seconds_per_job = float(job_time) / float(number_of_received_particle_results - 1);
			long seconds_remaining = float((input_refinement->number_of_particles * output_refinement->number_of_classes) - number_of_received_particle_results) * seconds_per_job;

			TimeRemaining time_remaining;

			if (seconds_remaining > 3600) time_remaining.hours = seconds_remaining / 3600;
			else time_remaining.hours = 0;

			if (seconds_remaining > 60) time_remaining.minutes = (seconds_remaining / 60) - (time_remaining.hours * 60);
			else time_remaining.minutes = 0;

			time_remaining.seconds = seconds_remaining - ((time_remaining.hours * 60 + time_remaining.minutes) * 60);
			my_parent->TimeRemainingText->SetLabel(wxString::Format("Time Remaining : %ih:%im:%is", time_remaining.hours, time_remaining.minutes, time_remaining.seconds));
		}


        // Add this result to the list of results to be plotted onto the angular plot
		if (current_class == 0)
		{
			my_parent->ShowRefinementResultsPanel->AngularPlotPanel->AddRefinementResult( &output_refinement->class_refinement_results[current_class].particle_refinement_results[current_particle]);
	         // Plot this new result onto the angular plot immediately if it's one of the first few results to come in. Otherwise, only plot at regular intervals.

	        if(my_parent->ShowRefinementResultsPanel->AngularPlotPanel->refinement_results_to_plot.Count() * my_parent->ShowRefinementResultsPanel->AngularPlotPanel->symmetry_matrices.number_of_matrices < 1500 || current_time - my_parent->time_of_last_result_update > 0)
	        {

	            my_parent->ShowRefinementResultsPanel->AngularPlotPanel->Refresh();
	            my_parent->time_of_last_result_update = current_time;
	        }

		}
	}
	else
	if (running_job_type == RECONSTRUCTION)
	{
		//wxPrintf("Got reconstruction job \n");
		number_of_received_particle_results++;
	//	wxPrintf("Received a reconstruction intermmediate result\n");

		long current_time = time(NULL);

		if (number_of_received_particle_results == 1)
		{
			time_of_last_update = 0;
			current_job_starttime = current_time;
		}
		else
		if (current_time - time_of_last_update >= 1)
		{
			time_of_last_update = current_time;
			int current_percentage = float(number_of_received_particle_results) / float(output_refinement->number_of_particles * output_refinement->number_of_classes) * 100.0;
			if (current_percentage > 100) current_percentage = 100;
			my_parent->ProgressBar->SetValue(current_percentage);
			long job_time = current_time - current_job_starttime;
			float seconds_per_job = float(job_time) / float(number_of_received_particle_results - 1);
			long seconds_remaining = float((input_refinement->number_of_particles * input_refinement->number_of_classes) - number_of_received_particle_results) * seconds_per_job;

			TimeRemaining time_remaining;
			if (seconds_remaining > 3600) time_remaining.hours = seconds_remaining / 3600;
			else time_remaining.hours = 0;

			if (seconds_remaining > 60) time_remaining.minutes = (seconds_remaining / 60) - (time_remaining.hours * 60);
			else time_remaining.minutes = 0;

			time_remaining.seconds = seconds_remaining - ((time_remaining.hours * 60 + time_remaining.minutes) * 60);
			my_parent->TimeRemainingText->SetLabel(wxString::Format("Time Remaining : %ih:%im:%is", time_remaining.hours, time_remaining.minutes, time_remaining.seconds));
		}


	}
	else
	if (running_job_type == MERGE)
	{
	//	wxPrintf("received merge result!\n");

		// add to the correct resolution statistics..

		int number_of_points = result_to_process->result_data[0];
		int class_number = int(result_to_process->result_data[1] + 0.5);
		int array_position = 2;
		float current_resolution;
		float fsc;
		float part_fsc;
		float part_ssnr;
		float rec_ssnr;

		wxPrintf("class_number = %i\n", class_number);
		// add the points..

		output_refinement->class_refinement_results[class_number - 1].class_resolution_statistics.Init(output_refinement->resolution_statistics_pixel_size, output_refinement->resolution_statistics_box_size);

		output_refinement->class_refinement_results[class_number - 1].class_resolution_statistics.FSC.ClearData();
		output_refinement->class_refinement_results[class_number - 1].class_resolution_statistics.part_FSC.ClearData();
		output_refinement->class_refinement_results[class_number - 1].class_resolution_statistics.part_SSNR.ClearData();
		output_refinement->class_refinement_results[class_number - 1].class_resolution_statistics.rec_SSNR.ClearData();


		for (int counter = 0; counter < number_of_points; counter++)
		{
			current_resolution = result_to_process->result_data[array_position];
			array_position++;
			fsc = result_to_process->result_data[array_position];
			array_position++;
			part_fsc = result_to_process->result_data[array_position];
			array_position++;
			part_ssnr = result_to_process->result_data[array_position];
			array_position++;
			rec_ssnr = result_to_process->result_data[array_position];
			array_position++;


			output_refinement->class_refinement_results[class_number - 1].class_resolution_statistics.FSC.AddPoint(current_resolution, fsc);
			output_refinement->class_refinement_results[class_number - 1].class_resolution_statistics.part_FSC.AddPoint(current_resolution, part_fsc);
			output_refinement->class_refinement_results[class_number - 1].class_resolution_statistics.part_SSNR.AddPoint(current_resolution, part_ssnr);
			output_refinement->class_refinement_results[class_number - 1].class_resolution_statistics.rec_SSNR.AddPoint(current_resolution, rec_ssnr);

		}

	}



}

void AutoRefinementManager::ProcessAllJobsFinished()
{

	if (running_job_type == REFINEMENT)
	{
		main_frame->job_controller.KillJob(my_parent->my_job_id);

		// calculate occupancies..
		if (output_refinement->percent_used < 99.99) output_refinement->UpdateOccupancies(false);
		else output_refinement->UpdateOccupancies(true);

		SetupReconstructionJob();
		RunReconstructionJob();

	}
	else
	if (running_job_type == RECONSTRUCTION)
	{
		main_frame->job_controller.KillJob(my_parent->my_job_id);
		//wxPrintf("Reconstruction has finished\n");
		SetupMerge3dJob();
		RunMerge3dJob();
	}
	else
	if (running_job_type == MERGE)
	{
		// launch drawer thread..

		OrthDrawerThread *result_thread = new OrthDrawerThread(my_parent, current_reference_filenames, wxString::Format("Iter. #%i", number_of_rounds_run + 1));

		if ( result_thread->Run() != wxTHREAD_NO_ERROR )
		{
			my_parent->WriteErrorText("Error: Cannot start result creation thread, results not displayed");
			delete result_thread;
		}

		int class_counter;

		main_frame->job_controller.KillJob(my_parent->my_job_id);

		VolumeAsset temp_asset;

		temp_asset.reconstruction_job_id = current_output_refinement_id;
		temp_asset.pixel_size = output_refinement->resolution_statistics_pixel_size;
		temp_asset.x_size = output_refinement->resolution_statistics_box_size;
		temp_asset.y_size = output_refinement->resolution_statistics_box_size;
		temp_asset.z_size = output_refinement->resolution_statistics_box_size;

		// add the volumes etc to the database..

		output_refinement->reference_volume_ids.Clear();
		refinement_package_asset_panel->all_refinement_packages[my_parent->RefinementPackageSelectPanel->GetSelection()].references_for_next_refinement.Clear();

		main_frame->current_project.database.Begin();
		main_frame->current_project.database.BeginVolumeAssetInsert();

		my_parent->WriteInfoText("");

		for (class_counter = 0; class_counter < output_refinement->number_of_classes; class_counter++)
		{
			temp_asset.asset_id = volume_asset_panel->current_asset_number;
			temp_asset.asset_name = wxString::Format("Auto #%li (Rnd. %i) - Class #%i", current_output_refinement_id, number_of_rounds_run + 1, class_counter + 1);
			temp_asset.filename = main_frame->current_project.volume_asset_directory.GetFullPath() + wxString::Format("/volume_%li_%i.mrc", output_refinement->refinement_id, class_counter + 1);
			output_refinement->reference_volume_ids.Add(temp_asset.asset_id);

			refinement_package_asset_panel->all_refinement_packages[my_parent->RefinementPackageSelectPanel->GetSelection()].references_for_next_refinement.Add(temp_asset.asset_id);
			main_frame->current_project.database.ExecuteSQL(wxString::Format("UPDATE REFINEMENT_PACKAGE_CURRENT_REFERENCES_%li SET VOLUME_ASSET_ID=%i WHERE CLASS_NUMBER=%i", current_refinement_package_asset_id, temp_asset.asset_id, class_counter + 1 ));


			volume_asset_panel->AddAsset(&temp_asset);
			main_frame->current_project.database.AddNextVolumeAsset(temp_asset.asset_id, temp_asset.asset_name, temp_asset.filename.GetFullPath(), temp_asset.reconstruction_job_id, temp_asset.pixel_size, temp_asset.x_size, temp_asset.y_size, temp_asset.z_size);
		}

		main_frame->current_project.database.EndVolumeAssetInsert();

		wxArrayFloat average_occupancies = output_refinement->UpdatePSSNR();

		my_parent->WriteInfoText("");

		if (output_refinement->number_of_classes > 1)
		{
			for (class_counter = 0; class_counter < output_refinement->number_of_classes; class_counter++)
			{
				my_parent->WriteInfoText(wxString::Format(wxT("Est. Res. Class %2i = %2.2f Å (%2.2f %%)"), class_counter + 1, output_refinement->class_refinement_results[class_counter].class_resolution_statistics.ReturnEstimatedResolution(), average_occupancies[class_counter]));
			}
		}
		else
		{
			my_parent->WriteInfoText(wxString::Format(wxT("Est. Res. = %2.2f Å"), output_refinement->class_refinement_results[0].class_resolution_statistics.ReturnEstimatedResolution()));
		}

		my_parent->WriteInfoText("");

		main_frame->current_project.database.AddRefinement(output_refinement);
		ShortRefinementInfo temp_info;
		temp_info = output_refinement;
		refinement_package_asset_panel->all_refinement_short_infos.Add(temp_info);

		// add this refinment to the refinement package..

		refinement_package_asset_panel->all_refinement_packages[my_parent->RefinementPackageSelectPanel->GetSelection()].last_refinment_id = output_refinement->refinement_id;
		refinement_package_asset_panel->all_refinement_packages[my_parent->RefinementPackageSelectPanel->GetSelection()].refinement_ids.Add(output_refinement->refinement_id);

		main_frame->current_project.database.ExecuteSQL(wxString::Format("UPDATE REFINEMENT_PACKAGE_ASSETS SET LAST_REFINEMENT_ID=%li WHERE REFINEMENT_PACKAGE_ASSET_ID=%li", output_refinement->refinement_id, current_refinement_package_asset_id));
		main_frame->current_project.database.ExecuteSQL(wxString::Format("INSERT INTO REFINEMENT_PACKAGE_REFINEMENTS_LIST_%li (REFINEMENT_NUMBER, REFINEMENT_ID) VALUES (%li, %li);", current_refinement_package_asset_id, main_frame->current_project.database.ReturnSingleLongFromSelectCommand(wxString::Format("SELECT MAX(REFINEMENT_NUMBER) FROM REFINEMENT_PACKAGE_REFINEMENTS_LIST_%li", current_refinement_package_asset_id)) + 1,  output_refinement->refinement_id));

		main_frame->current_project.database.Commit();
		volume_asset_panel->is_dirty = true;
		refinement_package_asset_panel->is_dirty = true;

	//		my_parent->SetDefaults();
		refinement_results_panel->is_dirty = true;

		my_parent->ShowRefinementResultsPanel->FSCResultsPanel->AddRefinement(output_refinement);

		if (my_parent->ShowRefinementResultsPanel->TopBottomSplitter->IsSplit() == false)
		{
			my_parent->ShowRefinementResultsPanel->TopBottomSplitter->SplitHorizontally(my_parent->ShowRefinementResultsPanel->TopPanel, my_parent->ShowRefinementResultsPanel->BottomPanel);
			my_parent->ShowRefinementResultsPanel->FSCResultsPanel->Show(true);
		}

		my_parent->Layout();
        main_frame->ClearAutoRefine3DScratch();

		//wxPrintf("Calling cycle refinement\n");
        main_frame->DirtyVolumes();
        main_frame->DirtyRefinements();
		CycleRefinement();
	}

}

void AutoRefinementManager::DoMasking()
{
	MyDebugAssertTrue(my_parent->AutoMaskYesRadio->GetValue() == true, "DoMasking called, when masking not ticked!");

	wxArrayString masked_filenames;
	wxFileName current_ref_filename;
	wxString current_masked_filename;

	for (int class_counter = 0; class_counter < current_reference_filenames.GetCount(); class_counter++)
	{
		current_ref_filename = current_reference_filenames.Item(class_counter);
		current_masked_filename = main_frame->ReturnAutoRefine3DScratchDirectory() + current_ref_filename.GetName();
		current_masked_filename += "_masked.mrc";

		masked_filenames.Add(current_masked_filename);
	}

	AutoRefine3DMaskerThread *mask_thread = new AutoRefine3DMaskerThread(my_parent, current_reference_filenames, masked_filenames, my_parent->MaskRadiusTextCtrl->ReturnValue(), input_refinement->resolution_statistics_pixel_size);

	if ( mask_thread->Run() != wxTHREAD_NO_ERROR )
	{
		my_parent->WriteErrorText("Error: Cannot start masking thread, masking will not be performed");
		delete mask_thread;
	}
	else
	{
		current_reference_filenames = masked_filenames;
		return; // just return, we will startup again whent he mask thread finishes.
	}
}

void AutoRefinementManager::CycleRefinement()
{
	percent_used_per_round.Add(current_percent_used);
	RefinementPackage *current_refinement_package = &refinement_package_asset_panel->all_refinement_packages.Item(my_parent->RefinementPackageSelectPanel->GetSelection());

	int class_counter;
	long particle_counter;

		// use highest resoluton..
	float best_res = FLT_MAX;
	float best_p143_res = FLT_MAX;
	float worse_res = -FLT_MAX;

	bool should_stop = false;
	bool did_resolution_improve;

	float current_0p5_resolution;
	float bleed_resolution;
	float res_for_next_round;
	float resolution_min_shells_after;
	float safe_resolution;
	float current_0p5_res_minus_bleed;

	int number_of_bleed_shells = ceil(output_refinement->resolution_statistics_box_size / (my_parent->MaskRadiusTextCtrl->ReturnValue() / output_refinement->resolution_statistics_pixel_size));

	// loop over all particles, to see if they were actually active, and if so whether they were global or not this number should be consistent for all classes, so only check class 1

	for (particle_counter = 0; particle_counter < output_refinement->number_of_particles; particle_counter++)
	{
		if (input_refinement->class_refinement_results[0].particle_refinement_results[particle_counter].image_is_active == 0 && output_refinement->class_refinement_results[0].particle_refinement_results[particle_counter].image_is_active == 1)
		{
			number_of_global_alignments[particle_counter]++;
			rounds_since_global_alignment[particle_counter] = 0;
		}
		else
		{
			rounds_since_global_alignment[particle_counter]++;
		}
	}

	for (class_counter = 0; class_counter < output_refinement->number_of_classes; class_counter++)
	{
		high_res_limit_per_round.Add(class_high_res_limits.Item(class_counter));

		current_0p5_resolution = output_refinement->class_refinement_results[class_counter].class_resolution_statistics.Return0p5Resolution(false);
		current_0p5_res_minus_bleed = output_refinement->class_refinement_results[class_counter].class_resolution_statistics.ReturnResolutionNShellsBefore(current_0p5_resolution, number_of_bleed_shells + 1);

		if (current_0p5_res_minus_bleed == 0) current_0p5_res_minus_bleed = class_high_res_limits[class_counter];

		bleed_resolution = output_refinement->class_refinement_results[class_counter].class_resolution_statistics.ReturnResolutionNShellsAfter(class_high_res_limits[class_counter], number_of_bleed_shells);
		resolution_min_shells_after = output_refinement->class_refinement_results[class_counter].class_resolution_statistics.ReturnResolutionNShellsAfter(class_high_res_limits[class_counter], output_refinement->resolution_statistics_box_size / 15 );

		if (bleed_resolution == 0) bleed_resolution = class_high_res_limits[class_counter];
		safe_resolution = bleed_resolution;

		res_for_next_round = std::max(resolution_min_shells_after, current_0p5_res_minus_bleed);
		if (res_for_next_round > class_high_res_limits.Item(class_counter)) res_for_next_round = class_high_res_limits.Item(class_counter);

		//if (res_for_next_round < 4.0) res_for_next_round = 4.0;

		class_high_res_limits[class_counter] = res_for_next_round;

	//	wxPrintf("\n\n\ncurrent_0p5_resolution = %.2f\n", current_0p5_resolution);
	//	wxPrintf("current_0p5_resolution_minus_bleed = %.2f\n", current_0p5_res_minus_bleed);
	//	wxPrintf("bleed_resolution = %.2f\n", bleed_resolution);
	//	wxPrintf("resolution_min_shells_after = %.2f\n", resolution_min_shells_after);
	//	wxPrintf("safe_resolution = %.2f\n", safe_resolution);
	//	wxPrintf("res_for_next_round = %.2f\n", res_for_next_round);
	//	wxPrintf("number_bleed_shells = %i\n\n\n\n", number_of_bleed_shells);

		if (output_refinement->class_refinement_results[class_counter].class_resolution_statistics.Return0p5Resolution() < best_res)
		{
			best_res = output_refinement->class_refinement_results[class_counter].class_resolution_statistics.Return0p5Resolution();
		}

		if (output_refinement->class_refinement_results[class_counter].class_resolution_statistics.ReturnEstimatedResolution() < best_p143_res)
		{
			best_p143_res = output_refinement->class_refinement_results[class_counter].class_resolution_statistics.ReturnEstimatedResolution();
		}

	}

	if (resolution_per_round.GetCount() > 0)
	{
		if (best_p143_res > resolution_per_round[resolution_per_round.GetCount() - 1] - 0.1) // the resolution did not improve, lets add more particles to the max
		{
			max_percent_used += max_percent_used * 0.5;
			if (max_percent_used > 100) max_percent_used = 100.0;
		}
	}

	resolution_per_round.Add(best_p143_res);
	//current_percent_used *= 2;

	float estimated_required_asym_units = 8000.0f * expf(75.0f / powf(best_p143_res,2));
	long wanted_number_of_asym_units = myroundint(estimated_required_asym_units) * output_refinement->number_of_classes;
	long number_of_asym_units = output_refinement->number_of_particles * ReturnNumberofAsymmetricUnits(current_refinement_package->symmetry);

	// what percentage is this.


	current_percent_used = (float(wanted_number_of_asym_units) / float(number_of_asym_units)) * 100.0;
	if (current_percent_used < start_percent_used) current_percent_used = start_percent_used;
	if (current_percent_used > 100.0) current_percent_used = 100.0;

	if (current_percent_used < max_percent_used) current_percent_used = max_percent_used;
	else (max_percent_used = current_percent_used);


	number_of_rounds_run++;

	//main_frame->DirtyRefinements();

	if (resolution_per_round.GetCount() >= 5)
	{
		should_stop = true;
		float round_resolution = resolution_per_round[resolution_per_round.GetCount() - 3];

		for (int round_counter = resolution_per_round.GetCount() - 2; round_counter <= resolution_per_round.GetCount() - 1; round_counter++)
		{
			if (resolution_per_round[round_counter] < round_resolution - 0.001) should_stop = false;
		}
	}

	if (this_is_the_final_round == true)
	{
		delete input_refinement;
		//delete output_refinement;
		my_parent->WriteBlueText("Resolution is stable - Auto refine is stopping.");
		my_parent->CancelAlignmentButton->Show(false);
		my_parent->FinishButton->Show(true);
		my_parent->TimeRemainingText->SetLabel("Time Remaining : Finished!");
		my_parent->ProgressBar->SetValue(100);
		my_parent->ProgressPanel->Layout();
	}
	else
	{
		if (should_stop == true)
		{
			this_is_the_final_round = true;
			current_percent_used = 100.0;
		}


		delete input_refinement;
		input_refinement = output_refinement;
		output_refinement = new Refinement;

		if (my_parent->AutoMaskYesRadio->GetValue() == true)
		{
			DoMasking();
		}
		else
		{
			SetupRefinementJob();
			RunRefinementJob();
		}
	}
}


void AutoRefine3DPanel::OnMaskerThreadComplete(wxThreadEvent& my_event)
{
	my_refinement_manager.OnMaskerThreadComplete();
}


void AutoRefinementManager::OnMaskerThreadComplete()
{
	//my_parent->WriteInfoText("Masking Finished");
	SetupRefinementJob();
	RunRefinementJob();
}

void AutoRefine3DPanel::OnOrthThreadComplete(MyOrthDrawEvent& my_event)
{

	Image *new_image = my_event.GetImage();

	if (new_image != NULL)
	{
		ShowRefinementResultsPanel->ShowOrthDisplayPanel->OpenImage(new_image, my_event.GetString(), true);

		if (ShowRefinementResultsPanel->LeftRightSplitter->IsSplit() == false)
		{
			ShowRefinementResultsPanel->LeftRightSplitter->SplitVertically(ShowRefinementResultsPanel->LeftPanel, ShowRefinementResultsPanel->RightPanel, 600);
			Layout();
		}
	}
}


wxThread::ExitCode AutoRefine3DMaskerThread::Entry()
{
	//  Read in the files, threshold them write them out again...

	Image input_image;
	ImageFile input_file;
	MRCFile output_file;

	float edge_value;

	for (int class_counter = 0; class_counter < input_files.GetCount(); class_counter++)
	{
		input_file.OpenFile(input_files.Item(class_counter).ToStdString(), false);
		input_image.ReadSlices(&input_file, 1, input_file.ReturnNumberOfSlices());
		input_file.CloseFile();

		// OLD MASKING

		/*
		input_image.CosineMask(mask_radius / pixel_size, 5 / pixel_size);
		edge_value = input_image.ReturnAverageOfRealValuesOnEdges();
		input_image.AddConstant(-edge_value);
		input_image.SetMinimumValue(0);
		*/

		input_image.SetMinimumValue(input_image.ReturnAverageOfRealValues());
		input_image.CosineMask(mask_radius / pixel_size, 1.0, false, true, 0.0);


		output_file.OpenFile(output_files.Item(class_counter).ToStdString(), true);
		input_image.WriteSlices(&output_file, 1, input_image.logical_z_dimension);
		output_file.CloseFile();
	}


	// send finished event..

	wxThreadEvent *my_thread_event = new wxThreadEvent(wxEVT_COMMAND_MASKERTHREAD_COMPLETED);
	wxQueueEvent(main_thread_pointer, my_thread_event);


	return (wxThread::ExitCode)0;     // success
}

