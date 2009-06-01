/**
 * \addtogroup rig
 * @{
 */

/**
 * \file src/tones.c
 * \brief CTCSS and DCS interface and tables
 * \author Stephane Fillod
 * \date 2000-2009
 */
/*
 *  Hamlib Interface - CTCSS and DCS interface and tables
 *  Copyright (c) 2000-2009 by Stephane Fillod
 *
 *	$Id: tones.c,v 1.11 2009-02-20 12:26:13 fillods Exp $
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include "hamlib/rig.h"
#include "tones.h"

#if !defined(_WIN32) && !defined(__CYGWIN__)

/**
 * 52 CTCSS sub-audible tones
 */
const tone_t full_ctcss_list[] = { FULL_CTCSS_LIST };

/**
 * 50 CTCSS sub-audible tones, from 67.0Hz to 254.1Hz
 *
 * \note Don't even think about changing a bit of this array, several
 * backends depend on it. If you need to, create a copy for your 
 * own caps. --SF
 */
const tone_t common_ctcss_list[] = { COMMON_CTCSS_LIST };

/**
 * 104 DCS codes
 */
const tone_t common_dcs_list[] = { COMMON_DCS_LIST };

/**
 * 106 DCS codes
 */
const tone_t full_dcs_list[] = { FULL_DCS_LIST };

#endif


#ifndef DOC_HIDDEN

#define CHECK_RIG_ARG(r) (!(r) || !(r)->caps || !(r)->state.comm_state)

#endif /* !DOC_HIDDEN */


/**
 * \brief set CTCSS sub-tone frequency
 * \param rig	The rig handle
 * \param vfo	The target VFO
 * \param tone	The tone to set to
 *
 *  Sets the current Continuous Tone Controlled Squelch System (CTCSS) 
 *  sub-audible tone frequency.
 *  \note  the \a tone integer is NOT in Hz, but in tenth of Hz! This way,
 *  if you want to set a subaudible tone of 88.5 Hz for example,
 *  then pass 885 to this function.
 *  Also, to disable Tone encoding, set \a tone to 0 (FIXME: not clear in API).
 *
 * \return RIG_OK if the operation has been sucessful, otherwise 
 * a negative value if an error occured (in which case, cause is 
 * set appropriately).
 *
 * \sa rig_get_ctcss_tone(), rig_set_ctcss_sql()
 */

int HAMLIB_API rig_set_ctcss_tone(RIG *rig, vfo_t vfo, tone_t tone)
{
	const struct rig_caps *caps;
	int retcode;
	vfo_t curr_vfo;

	if (CHECK_RIG_ARG(rig))
		return -RIG_EINVAL;

	caps = rig->caps;

	if (caps->set_ctcss_tone == NULL)
		return -RIG_ENAVAIL;

	if ((caps->targetable_vfo&RIG_TARGETABLE_PURE) || 
				vfo == RIG_VFO_CURR || vfo == rig->state.current_vfo)
		return caps->set_ctcss_tone(rig, vfo, tone);

	if (!caps->set_vfo)
		return -RIG_ENTARGET;
	curr_vfo = rig->state.current_vfo;
	retcode = caps->set_vfo(rig, vfo);
	if (retcode != RIG_OK)
		return retcode;

	retcode = caps->set_ctcss_tone(rig, vfo, tone);
	caps->set_vfo(rig, curr_vfo);
	return retcode;
}

/**
 * \brief get the current CTCSS sub-tone frequency
 * \param rig	The rig handle
 * \param vfo	The target VFO
 * \param tone	The location where to store the current tone
 *
 *  Retrieves the current Continuous Tone Controlled Squelch System (CTCSS) 
 *  sub-audible tone frequency.
 *  \note the \a *tone integer is NOT in Hz, but in tenth of Hz! This way,
 *  if the function rig_get_ctcss_tone() returns a subaudible tone of 885
 *  for example, then the real tone is 88.5 Hz. 
 *  Also, a value of 0 for \a *tone means the Tone encoding is disabled.
 *
 * \return RIG_OK if the operation has been sucessful, otherwise 
 * a negative value if an error occured (in which case, cause is 
 * set appropriately).
 *
 * \sa rig_set_ctcss_tone(), rig_get_ctcss_sql()
 */
int HAMLIB_API rig_get_ctcss_tone(RIG *rig, vfo_t vfo, tone_t *tone)
{
	const struct rig_caps *caps;
	int retcode;
	vfo_t curr_vfo;

	if (CHECK_RIG_ARG(rig) || !tone)
		return -RIG_EINVAL;

	caps = rig->caps;

	if (caps->get_ctcss_tone == NULL)
		return -RIG_ENAVAIL;

	if ((caps->targetable_vfo&RIG_TARGETABLE_PURE) || 
				vfo == RIG_VFO_CURR || vfo == rig->state.current_vfo)
		return caps->get_ctcss_tone(rig, vfo, tone);

	if (!caps->set_vfo)
		return -RIG_ENTARGET;
	curr_vfo = rig->state.current_vfo;
	retcode = caps->set_vfo(rig, vfo);
	if (retcode != RIG_OK)
		return retcode;

	retcode = caps->get_ctcss_tone(rig, vfo, tone);
	caps->set_vfo(rig, curr_vfo);
	return retcode;
}

/**
 * \brief set the current encoding DCS code
 * \param rig	The rig handle
 * \param vfo	The target VFO
 * \param code	The tone to set to
 *
 * Sets the current encoding Digitally-Coded Squelch code.
 *
 * \return RIG_OK if the operation has been sucessful, otherwise 
 * a negative value if an error occured (in which case, cause is 
 * set appropriately).
 *
 * \sa rig_get_dcs_code(), rig_set_dcs_sql()
 */

int HAMLIB_API rig_set_dcs_code(RIG *rig, vfo_t vfo, tone_t code)
{
	const struct rig_caps *caps;
	int retcode;
	vfo_t curr_vfo;

	if (CHECK_RIG_ARG(rig))
		return -RIG_EINVAL;

	caps = rig->caps;

	if (caps->set_dcs_code == NULL)
		return -RIG_ENAVAIL;

	if ((caps->targetable_vfo&RIG_TARGETABLE_PURE) || 
				vfo == RIG_VFO_CURR || vfo == rig->state.current_vfo)
		return caps->set_dcs_code(rig, vfo, code);

	if (!caps->set_vfo)
		return -RIG_ENTARGET;
	curr_vfo = rig->state.current_vfo;
	retcode = caps->set_vfo(rig, vfo);
	if (retcode != RIG_OK)
		return retcode;

	retcode = caps->set_dcs_code(rig, vfo, code);
	caps->set_vfo(rig, curr_vfo);
	return retcode;
}

/**
 * \brief get the current encoding DCS code
 * \param rig	The rig handle
 * \param vfo	The target VFO
 * \param code	The location where to store the current tone
 *
 * Retrieves the current encoding Digitally-Coded Squelch code. 
 *
 * \return RIG_OK if the operation has been sucessful, otherwise 
 * a negative value if an error occured (in which case, cause is 
 * set appropriately).
 *
 * \sa rig_set_dcs_code(), rig_get_dcs_sql()
 */
int HAMLIB_API rig_get_dcs_code(RIG *rig, vfo_t vfo, tone_t *code)
{
	const struct rig_caps *caps;
	int retcode;
	vfo_t curr_vfo;

	if (CHECK_RIG_ARG(rig) || !code)
		return -RIG_EINVAL;

	caps = rig->caps;

	if (caps->get_dcs_code == NULL)
		return -RIG_ENAVAIL;

	if ((caps->targetable_vfo&RIG_TARGETABLE_PURE) || 
				vfo == RIG_VFO_CURR || vfo == rig->state.current_vfo)
		return caps->get_dcs_code(rig, vfo, code);

	if (!caps->set_vfo)
		return -RIG_ENTARGET;
	curr_vfo = rig->state.current_vfo;
	retcode = caps->set_vfo(rig, vfo);
	if (retcode != RIG_OK)
		return retcode;

	retcode = caps->get_dcs_code(rig, vfo, code);
	caps->set_vfo(rig, curr_vfo);
	return retcode;
}

/**
 * \brief set CTCSS squelch
 * \param rig	The rig handle
 * \param vfo	The target VFO
 * \param tone	The PL tone to set the squelch to
 *
 *  Sets the current Continuous Tone Controlled Squelch System (CTCSS)
 *  sub-audible *squelch* tone.
 *  \note \a tone is NOT in Hz, but in tenth of Hz! This way,
 *  if you want to set subaudible squelch tone of 88.5 Hz for example,
 *  then pass 885 to this function.
 *  Also, to disable Tone squelch, set \a tone to 0 (FIXME: not clear in API).
 *
 * \return RIG_OK if the operation has been sucessful, otherwise 
 * a negative value if an error occured (in which case, cause is 
 * set appropriately).
 *
 * \sa rig_get_ctcss_sql(), rig_set_ctcss_tone()
 */

int HAMLIB_API rig_set_ctcss_sql(RIG *rig, vfo_t vfo, tone_t tone)
{
	const struct rig_caps *caps;
	int retcode;
	vfo_t curr_vfo;

	if (CHECK_RIG_ARG(rig))
		return -RIG_EINVAL;

	caps = rig->caps;

	if (caps->set_ctcss_sql == NULL)
		return -RIG_ENAVAIL;

	if ((caps->targetable_vfo&RIG_TARGETABLE_PURE) || 
				vfo == RIG_VFO_CURR || vfo == rig->state.current_vfo)
		return caps->set_ctcss_sql(rig, vfo, tone);

	if (!caps->set_vfo)
		return -RIG_ENTARGET;
	curr_vfo = rig->state.current_vfo;
	retcode = caps->set_vfo(rig, vfo);
	if (retcode != RIG_OK)
		return retcode;

	retcode = caps->set_ctcss_sql(rig, vfo, tone);
	caps->set_vfo(rig, curr_vfo);
	return retcode;
}

/**
 * \brief get the current CTCSS squelch
 * \param rig	The rig handle
 * \param vfo	The target VFO
 * \param tone	The location where to store the current tone
 *
 *  Retrieves the current Continuous Tone Controlled Squelch System (CTCSS) 
 *  sub-audible *squelch* tone.
 *  \note \a *tone is NOT in Hz, but in tenth of Hz! This way,
 *  if the function rig_get_ctcss_sql() returns a subaudible tone of 885
 *  for example, then the real tone is 88.5 Hz. 
 *  Also, a value of 0 for \a tone means the Tone squelch is disabled.
 *
 * \return RIG_OK if the operation has been sucessful, otherwise 
 * a negative value if an error occured (in which case, cause is 
 * set appropriately).
 *
 * \sa rig_set_ctcss_sql(), rig_get_ctcss_tone()
 */
int HAMLIB_API rig_get_ctcss_sql(RIG *rig, vfo_t vfo, tone_t *tone)
{
	const struct rig_caps *caps;
	int retcode;
	vfo_t curr_vfo;

	if (CHECK_RIG_ARG(rig) || !tone)
		return -RIG_EINVAL;

	caps = rig->caps;

	if (caps->get_ctcss_sql == NULL)
		return -RIG_ENAVAIL;

	if ((caps->targetable_vfo&RIG_TARGETABLE_PURE) ||
			vfo == RIG_VFO_CURR || vfo == rig->state.current_vfo)
		return caps->get_ctcss_sql(rig, vfo, tone);

	if (!caps->set_vfo)
		return -RIG_ENTARGET;
	curr_vfo = rig->state.current_vfo;
	retcode = caps->set_vfo(rig, vfo);
	if (retcode != RIG_OK)
		return retcode;

	retcode = caps->get_ctcss_sql(rig, vfo, tone);
	caps->set_vfo(rig, curr_vfo);
	return retcode;
}

/**
 * \brief set the current DCS code
 * \param rig	The rig handle
 * \param vfo	The target VFO
 * \param code	The tone to set to
 *
 * Sets the current Digitally-Coded *Squelch* code.
 *
 * \return returns RIG_OK if the operation has been sucessful, ortherwise
 * a negative value if an error occured (in which case, cause is set
 * appropriately).
 *
 * \sa rig_get_dcs_sql(), rig_set_dcs_code()
 */

int HAMLIB_API rig_set_dcs_sql(RIG *rig, vfo_t vfo, tone_t code)
{
	const struct rig_caps *caps;
	int retcode;
	vfo_t curr_vfo;

	if (CHECK_RIG_ARG(rig))
		return -RIG_EINVAL;

	caps = rig->caps;

	if (caps->set_dcs_sql == NULL)
		return -RIG_ENAVAIL;

	if ((caps->targetable_vfo&RIG_TARGETABLE_PURE) ||
			vfo == RIG_VFO_CURR || vfo == rig->state.current_vfo)
		return caps->set_dcs_sql(rig, vfo, code);

	if (!caps->set_vfo)
		return -RIG_ENTARGET;
	curr_vfo = rig->state.current_vfo;
	retcode = caps->set_vfo(rig, vfo);
	if (retcode != RIG_OK)
		return retcode;

	retcode = caps->set_dcs_sql(rig, vfo, code);
	caps->set_vfo(rig, curr_vfo);
	return retcode;
}

/**
 * \brief get the current DCS code
 * \param rig	The rig handle
 * \param vfo	The target VFO
 * \param code	The location where to store the current tone
 *
 * Retrieves the current Digitally-Coded *Squelch* code. 
 *
 * \return RIG_OK if the operation has been sucessful, ortherwise 
 * a negative value if an error occured (in which case, cause is 
 * set appropriately).
 *
 * \sa rig_set_dcs_sql(), rig_get_dcs_code()
 */
int HAMLIB_API rig_get_dcs_sql(RIG *rig, vfo_t vfo, tone_t *code)
{
	const struct rig_caps *caps;
	int retcode;
	vfo_t curr_vfo;

	if (CHECK_RIG_ARG(rig) || !code)
		return -RIG_EINVAL;

	caps = rig->caps;

	if (caps->get_dcs_sql == NULL)
		return -RIG_ENAVAIL;

	if ((caps->targetable_vfo&RIG_TARGETABLE_PURE) ||
			vfo == RIG_VFO_CURR || vfo == rig->state.current_vfo)
		return caps->get_dcs_sql(rig, vfo, code);

	if (!caps->set_vfo)
		return -RIG_ENTARGET;
	curr_vfo = rig->state.current_vfo;
	retcode = caps->set_vfo(rig, vfo);
	if (retcode != RIG_OK)
		return retcode;

	retcode = caps->get_dcs_sql(rig, vfo, code);
	caps->set_vfo(rig, curr_vfo);
	return retcode;
}

/*! @} */
