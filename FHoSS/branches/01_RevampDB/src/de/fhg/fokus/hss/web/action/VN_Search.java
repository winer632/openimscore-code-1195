/*
  *  Copyright (C) 2004-2007 FhG Fokus
  *
  * This file is part of Open IMS Core - an open source IMS CSCFs & HSS
  * implementation
  *
  * Open IMS Core is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  *
  * For a license to use the Open IMS Core software under conditions
  * other than those described here, or to purchase support for this
  * software, please contact Fraunhofer FOKUS by e-mail at the following
  * addresses:
  *     info@open-ims.org
  *
  * Open IMS Core is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * It has to be noted that this Open Source IMS Core System is not
  * intended to become or act as a product in a commercial context! Its
  * sole purpose is to provide an IMS core reference implementation for
  * IMS technology testing and IMS application prototyping for research
  * purposes, typically performed in IMS test-beds.
  *
  * Users of the Open Source IMS Core System have to be aware that IMS
  * technology may be subject of patents and licence terms, as being
  * specified within the various IMS-related IETF, ITU-T, ETSI, and 3GPP
  * standards. Thus all Open IMS Core users have to take notice of this
  * fact and have to agree to check out carefully before installing,
  * using and extending the Open Source IMS Core System, if related
  * patents and licenses may become applicable to the intended usage
  * context. 
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  
  * 
  */

package de.fhg.fokus.hss.web.action;

import java.util.LinkedList;
import java.util.List;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts.action.Action;
import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionForward;
import org.apache.struts.action.ActionMapping;
import org.hibernate.Query;
import org.hibernate.Session;

import de.fhg.fokus.hss.db.model.ApplicationServer;
import de.fhg.fokus.hss.db.model.IMPI;
import de.fhg.fokus.hss.db.model.VisitedNetwork;
import de.fhg.fokus.hss.db.op.ApplicationServer_DAO;
import de.fhg.fokus.hss.db.op.IMPI_DAO;
import de.fhg.fokus.hss.db.op.VisitedNetwork_DAO;
import de.fhg.fokus.hss.db.hibernate.*;
import de.fhg.fokus.hss.web.form.AS_SearchForm;
import de.fhg.fokus.hss.web.form.IMPI_SearchForm;
import de.fhg.fokus.hss.web.form.VN_SearchForm;
import de.fhg.fokus.hss.web.util.WebConstants;

/**
 * @author adp dot fokus dot fraunhofer dot de 
 * Adrian Popescu / FOKUS Fraunhofer Institute
 */


public class VN_Search extends Action{
	
	public ActionForward execute(ActionMapping mapping, ActionForm actionForm,
			HttpServletRequest request, HttpServletResponse reponse) {
		
		VN_SearchForm form = (VN_SearchForm) actionForm;
		Object [] queryResult = null;
		VisitedNetwork uniqueResult = null;
		ActionForward forward = null;
		
		int rowsPerPage = Integer.parseInt(form.getRowsPerPage());
		int currentPage = Integer.parseInt(form.getCrtPage()) - 1;
		int firstResult = currentPage * rowsPerPage;		

		try{
			HibernateUtil.beginTransaction();
			Session session = HibernateUtil.getCurrentSession();
		
			if (form.getId_vn() != null && !form.getId_vn().equals("")){
				uniqueResult = VisitedNetwork_DAO.get_by_ID(session, Integer.parseInt(form.getId_vn()));
			}
			else if (form.getIdentity() != null && !form.getIdentity().equals("")){
				queryResult = VisitedNetwork_DAO.get_by_Wildcarded_Identity(session, form.getIdentity(), firstResult, rowsPerPage);
			}
			else{
				queryResult = VisitedNetwork_DAO.get_all(session, firstResult, rowsPerPage);
			}
		
			int maxPages = 1;
			if (queryResult != null){
				// more than one result
				maxPages = ((((Integer)queryResult[0]).intValue() - 1) / rowsPerPage) + 1;
				request.setAttribute("resultList", (List)queryResult[1]);
			}
			else {
				List list = new LinkedList();
				if (uniqueResult != null){
					list.add(uniqueResult);
				}
				request.setAttribute("resultList", list);
			}
			
			if (currentPage > maxPages){
				currentPage = 0;
			}
			form.reset(mapping, request);
			request.setAttribute("maxPages", String.valueOf(maxPages));
			request.setAttribute("currentPage", String.valueOf(currentPage));
			request.setAttribute("rowPerPage", String.valueOf(rowsPerPage));
			forward = mapping.findForward(WebConstants.FORWARD_SUCCESS);
		}
		catch(DatabaseException e){
			forward = mapping.findForward(WebConstants.FORWARD_FAILURE);
		}
		finally{
			HibernateUtil.commitTransaction();
			HibernateUtil.closeSession();
		}

		return forward;
	}
}