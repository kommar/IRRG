#include "System.h"

#include "CommandParamParser.h"
#include "OptimizationCommand.h"
#include "RandUtils.h"
#include "StringCommandParam.h"
#include "UIntCommandParam.h"
#include "FloatCommandParam.h"

#include <ctime>




uint32_t CSystem::iERROR_CODE_SYSTEM_IN_FILE_NOT_FOUND = CError::iADD_ERROR("iERROR_CODE_SYSTEM_IN_FILE_NOT_FOUND");
uint32_t CSystem::iERROR_CODE_SYSTEM_SETTNGS_FILE_NOT_FOUND = CError::iADD_ERROR("iERROR_CODE_SYSTEM_SETTNGS_FILE_NOT_FOUND");

CSystem::CSystem(int iArgCount, char **ppcArgValues, CError *pcError)
{
	pf_log = nullptr;

	*pcError = e_read_arguments_from_file(iArgCount, ppcArgValues);

	if (!*pcError)
	{
		f_result_summary.open(s_result_summary_file);

		if (f_result_summary.good())
		{
			v_register_commands();
		}//if (f_result_summary.good())
		else
		{
			pcError->vSetError(CError::iERROR_CODE_SYSTEM_WRONG_FILE_PATH, s_result_summary_file);
		}//else if (f_result_summary.good())
	}//if (!*pcError)
}//CSystem::CSystem(int iArgCount, char **ppcArgValues, CError *pcError)

CSystem::~CSystem()
{
	if  (pf_log != nullptr)  delete  pf_log;

	m_arguments.clear();

	for (unordered_map<CString, CCommandExecution*>::iterator i_it = m_commands.begin(); i_it != m_commands.end(); i_it++)
	{
		delete i_it->second;
	}//for (unordered_map<CString, CCommandExecution*>::iterator i_it = m_commands.begin(); i_it != m_commands.end(); i_it++)

	m_commands.clear();

	f_result_summary.close();

	f_settings.close();
}//CSystem::~CSystem()

CError CSystem::eRun()
{
	CError c_error;
	
	time_t t_start = time(nullptr);

	CCommandParamParser c_command_param_parser(&m_arguments);
	
	CStringCommandParam c_dest_file_param(SYSTEM_ARGUMENT_DESTINATION_FILE);
	CStringCommandParam c_mode_param(SYSTEM_ARGUMENT_MODE);
	CUIntCommandParam c_random_seed_param(SYSTEM_ARGUMENT_RANDOM_SEED, false);
	CFloatCommandParam c_log_system_param(SYSTEM_ARGUMENT_LOG_SYSTEM_OFFSET, false);

	
	CString  s_dest_file_param = c_dest_file_param.sGetValue(&f_settings, &c_error);

	if (!c_error)
	{
		c_error = e_create_log_file(s_dest_file_param, &s_dest_file_param);

		if (!c_error)
		{
			int  i_log_system_off;
			i_log_system_off = (int) ::round(c_log_system_param.fGetValue(&f_settings, &c_error));
			if (c_log_system_param.bHasValue() == false)  i_log_system_off = 0;
			c_log.vLogSystemToShow(i_log_system_off);
			
			CString s_mode = c_mode_param.sGetValue(&f_settings, &c_error);

			if (!c_error)
			{
				i_random_seed = c_random_seed_param.iGetValue(&f_settings, &c_error);

				if (!c_error)
				{
					if (c_random_seed_param.bHasValue())
					{
						i_random_seed = c_random_seed_param.iGetValue();
						RandUtils::vInit(i_random_seed);
					}//if (c_random_seed_param.bHasValue())
					else
					{
						i_random_seed = RandUtils::iInit();
					}//else if (c_random_seed_param.bHasValue())

					FILE  *pf_file;
					CString  s_buf;
					s_buf.Format("z_seed_%d.txt", i_random_seed);
					pf_file = fopen(s_buf, "w+");
					fprintf(pf_file, "%d", i_random_seed);
					fclose(pf_file);


					if (m_commands.count(s_mode) > 0)
					{
						c_error = m_commands.at(s_mode)->eExecute(&c_command_param_parser, i_random_seed);

						if (!c_error)
						{
							time_t t_stop = time(nullptr);
							uint32_t i_running_time = (uint32_t)(t_stop - t_start);

							CString s_summary;
							s_summary.Format("\t%u\t%u", i_running_time, i_random_seed);

							f_result_summary << s_summary << endl;
						}//if (!c_error)
					}//if (m_commands.count(s_mode) > 0)
					else
					{
						c_error.vSetError(CError::iERROR_CODE_SYSTEM_MODE_NOT_FOUND, s_mode);
					}//else if (m_commands.count(s_mode) > 0)

					::remove(s_buf);
					//pf_file = fopen(s_buf, "w+");
					//fclose(pf_file);
				}//if (!c_error)
			}//if (!c_error)

			c_error = c_log.eSave(pf_log, i_log_system_off);
			//c_error = c_log.eSave(s_dest_file_param + "pf.txt", LOG_SYSTEM_PARETO_FRONT);

			delete  pf_log;
			pf_log = nullptr;

			//c_log.eSave(s_log_file, i_log_system_off);
		}//if (!c_error)
	}//if (!c_error)

	return c_error;
}//CError CSystem::eRun()




void CSystem::v_register_commands()
{
	v_register_command(new COptimizationCommand(&f_settings, &c_log, &f_result_summary));
	v_register_command(new CDecompositionCommand(&f_settings, &c_log, &f_result_summary));
}//void CSystem::v_register_commands()




CError CSystem::e_create_log_file(CString  sFileName, CString  *psFinalFilename)
{ 
	CError c_err;

	int  i_file_counter;
	CString  s_effective_file_name;
	i_file_counter = 0;
	s_effective_file_name.Format("%s_%.3d.txt", sFileName, i_file_counter);
		
	while (::PathFileExists(s_effective_file_name) == true)
	{
		i_file_counter++;
		s_effective_file_name.Format("%s_%.3d.txt", sFileName, i_file_counter);
	}//while  (System::IO::File::Exists(s_file_name)  ==  true)

	pf_log = new ofstream(s_effective_file_name);
	//MessageBox(NULL, "test", "test", MB_OK);

	*psFinalFilename = s_effective_file_name;

	c_log.vSetLogFile(s_effective_file_name);

	return(c_err);
}//CError CSystem::e_create_log_file(CString  sFileName)


CError CSystem::e_read_arguments_from_file(int iArgCount, char **ppcArgValues)
{
	CError c_err;

	CString  s_argument(*(ppcArgValues+1));
	//::MessageBox(NULL, s_argument, s_argument, MB_OK);


	if  (s_argument  !=  "")
    {
		CString  s_file_name, s_buf;
		FILE  *pf_in_file;

		s_result_summary_file.Format("%s_out.txt", s_argument);
		s_log_file.Format("%s_log.txt", s_argument);
		s_file_name.Format("%s_entry.txt", s_argument);


		pf_in_file = fopen(s_file_name, "r");
		if (pf_in_file == NULL)
		{
			c_err.vSetError(iERROR_CODE_SYSTEM_IN_FILE_NOT_FOUND, s_file_name);
			return(c_err);
		}//if  (pf_in_file  ==  NULL)


		CString  s_in_data;
		s_in_data = Tools::sReadLine(pf_in_file);
		fclose(pf_in_file);

		e_load_settings(s_in_data);
    }//if  (s_argument  !=  "")*/
	else
	{

		//PRW: tymczasowe - do wywalenia
		
		CString  s_file_name, s_buf;
		FILE  *pf_in_file;

		s_result_summary_file = "summary.txt";
		s_log_file = "log.txt";
		s_file_name = "C:\\Projekty\\Visual Studio\\2017\\z_prw_island_ga\\x64\\Release\\decomposition_settings.txt";


		e_load_settings(s_file_name);
	}//esle  if  (s_argument  !=  "")*/


	return(c_err);
}//CError CSystem::e_read_arguments_from_file(int iArgCount, char **ppcArgValues)



CError CSystem::e_load_settings(CString sSettingsFile)
{
	CError c_err;

	f_settings.open(sSettingsFile);

	if (!f_settings.good())
	{
		c_err.vSetError(iERROR_CODE_SYSTEM_SETTNGS_FILE_NOT_FOUND, sSettingsFile);
	}//if (!f_settings.good())

	//FILE  *pf_sett_file;
	//pf_sett_file = fopen(sSettingsFile, "r");
 //   if  (pf_sett_file  ==  NULL)
 //   {
	//	c_err.vSetError(iERROR_CODE_SYSTEM_SETTNGS_FILE_NOT_FOUND, sSettingsFile);
 //       return(c_err);
 //   }//if  (pf_in_file  ==  NULL)


	//CString  s_setting_name,  s_value,  s_setting_log;

	//c_log.vPrintLine("---- PARAMS ----");

	//while  (!feof(pf_sett_file))
	//{
	//	s_value  =  Tools::sReadLine(pf_sett_file, &s_setting_name);

	//	if (s_setting_name.IsEmpty() == false)
	//	{
	//		m_arguments.insert(pair<const CString, CString>(s_setting_name, s_value));
	//	
	//		s_setting_log.Format("%s: %s", s_setting_name, s_value);
	//		c_log.vPrintLine(s_setting_log);
	//	}//if (s_setting_name.IsEmpty() == false)

	//	//Tools::vShow(s_value);
	//}//while  (!feof(pf_sett_file))

	//c_log.vPrintEmptyLine();

	//fclose(pf_sett_file);

	return(c_err);
}//CError CSystem::e_load_settings(CString sSettingsFile)


/*

CError  CVirusGA::eLoadSettings(FILE  *pfSettings)
{
	CError  c_err;
	CString  s_setting_name,  s_buf;

	i_gen  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_MAX_TIME)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_MAX_TIME, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_pop_size  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_POP_SIZE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_POP_SIZE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_infected_pop_size  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_INFECTED_POP_SIZE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_INFECTED_POP_SIZE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_prob_cross  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_PROB_CROSS)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_PROB_CROSS, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	b_static_cross  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_STATIC_CROSS)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_STATIC_CROSS, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_comp_templates_crossing_type  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_CROSS_TYPE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_CROSS_TYPE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_prob_fit_diffr_tournament  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_CHANCE_FOR_3_PARENT_DIFFR_TOURNAMENT)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_CHANCE_FOR_3_PARENT_DIFFR_TOURNAMENT, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)

	
	b_glue_infection_rows  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_GLUE_INFECTIONS)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_GLUE_INFECTIONS, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	b_fitness_change_templ_check  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_TEMPL_FITNESS_CHECK)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_TEMPL_FITNESS_CHECK, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_max_infections_considered_at_crossing  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_PATTERN_POOL_SIZE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_PATTERN_POOL_SIZE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_the_same_template_check  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_SAME_PATTERN_CHECK)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_SAME_PATTERN_CHECK, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_preferred_template_length  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_PREFERRED_PATTERN_LENGTH)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_PREFERRED_PATTERN_LENGTH, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)



	i_minimal_template_length  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_MINIMAL_PATTERN_LEN)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_MINIMAL_PATTERN_LEN, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)



	b_pattern_check_length_entrophy  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_LEN_OR_ENTROPHY)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_LEN_OR_ENTROPHY, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	b_use_templates_at_virus_pop_init  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_USE_TEMPL_AT_VIRUS_INIT)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_USE_TEMPL_AT_VIRUS_INIT, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)



	b_vir_init_different_to_template  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_INIT_DIFFERENT_TO_TEMPLATE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_INIT_DIFFERENT_TO_TEMPLATE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)



	i_virus_gen  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_GENERATIONS)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_GENERATIONS, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_virus_pop_size  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_POP)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_POP, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_pop_reduction_juxtapositional  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_POP_RED)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_POP_RED, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_vir_prob_cut  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_PROB_CUT)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_PROB_CUT, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_vir_prob_splice  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_PROB_SPLICE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_PROB_SPLICE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_vir_prob_mut  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_PROB_MUT)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_PROB_MUT, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_prob_mut_rem_gene  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_REM_GENE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_REM_GENE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	d_prob_mut_add_gene  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_ADD_GENE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_ADD_GENE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_virginity_rounds  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIRGIN_ROUNDS)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIRGIN_ROUNDS, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_VIRGIN_ROUNDS)


	i_vir_min_init_length  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_MIN_VIRUS_INIT_LENGTH)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_MIN_VIRUS_INIT_LENGTH, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_MIN_VIRUS_INIT_LENGTH)


	i_vir_max_init_length  =  Tools::iReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_MAX_VIRUS_INIT_LENGTH)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_MAX_VIRUS_INIT_LENGTH, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_MAX_VIRUS_INIT_LENGTH)




	i_ct_add_remove_strategy  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIR_CT_STRATEGY)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIR_CT_STRATEGY, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)



	i_feno_viruses  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_FENO_VIRUS)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_FENO_VIRUS, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_GENERATIONS)


	i_cut_type  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_CUT_TYPE)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_CUT_TYPE, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_CUT_TYPE)


	i_genotype_step_optimization  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_GENO_STEP_OPTIMIZATION)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_GENO_STEP_OPTIMIZATION, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_CUT_TYPE)


	i_pattern_extract  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_PATTERN_EXTRACT)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_PATTERN_EXTRACT, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_CUT_TYPE)


	i_ct_cross  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_CROSS_CT)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_CROSS_CT, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_CROSS_CT)


	i_separate_virus_pattern_and_random_init  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIRUS_INITED_SEPARATE_PATTERN_AND_RANDOM)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIRUS_INITED_SEPARATE_PATTERN_AND_RANDOM, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_VIRUS_INITED_SEPARATE_PATTERN_AND_RANDOM)


	i_virus_pure_pattern_init  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_VIRUS_PATTERN_INITED)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_VIRUS_PATTERN_INITED, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_VIRUS_PATTERN_INITED)


	i_add_ct_if_best_not_better  =  Tools::dReadLine(pfSettings, &s_setting_name);
	if  (s_setting_name  !=  VGA_PARAM_CT_ADD_CT_IF_BEST_IS_NOT_BETTER)
	{
		s_buf.Format("Error at reading (%s) parameter read:(%s)", VGA_PARAM_CT_ADD_CT_IF_BEST_IS_NOT_BETTER, s_setting_name);
		c_err.vPutError(s_buf);
		return(c_err);	
	}//if  (s_setting_name  !=  VGA_PARAM_CT_ADD_CT_IF_BEST_IS_NOT_BETTER)

	


	

	return(c_err);
}//CError  CVirusGA::eLoadSettings(FILE  *pfSettings)



*/