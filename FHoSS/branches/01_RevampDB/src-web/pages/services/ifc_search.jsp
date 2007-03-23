<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
	pageEncoding="ISO-8859-1"%>
<%@ taglib uri="http://jakarta.apache.org/struts/tags-bean"
	prefix="bean"%>
<%@ taglib uri="http://jakarta.apache.org/struts/tags-html"
	prefix="html"%>
<%@ taglib uri="http://jakarta.apache.org/struts/tags-logic"
	prefix="logic"%>
<%@ page import="de.fhg.fokus.hss.util.SecurityPermissions" %>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<link rel="stylesheet" type="text/css"
	href="/hss.web.console/style/fokus_ngni.css">

<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title><bean:message key="result.title" /></title>
</head>
<body>

<center>
<table>
<tr>
	<td><br/><br/><h1>Initial Filter Criteria - Search </h1><br/><br/></td>
</tr>
</table>
</center>

<html:form action="IFC_Search">
<center>
		<table>
			<tr>
				<td>Search by ID</td>
				<td><html:text property="id_ifc" value="" styleClass="inputbox"/></td>
			</tr>
			<tr>
				<td>Search by Name</td>
				<td><html:text property="name" value="" styleClass="inputbox"/></td>
			</tr>
			<tr>
				<td>Search by AS Name</td>
				<td><html:text property="name_application_server" value="" styleClass="inputbox"/></td>
			</tr>
			<tr>
				<td>Search by TP Name</td>
				<td><html:text property="name_tp" value="" styleClass="inputbox"/></td>
			</tr>
		</table>
		<table>		
			<tr>
				<td><br/><html:submit property="search" value="Search" /></td>
			</tr>				
		</table>		
</center>
</html:form>
</body>
</html>
